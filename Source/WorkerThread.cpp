#include "WorkerThread.hpp"
#include <cstdio>

using namespace std;

WorkerThread::WorkerThread(void)
  : tasks(ID_MAX), mtx(ID_MAX), cond(ID_MAX), state(ID_MAX), event(ID_MAX)
{
    wakeup();
}

WorkerThread::WorkerThread(int th_cnt)
  : tasks(th_cnt), mtx(th_cnt), cond(th_cnt), state(th_cnt), event(th_cnt)
{
    wakeup();
}

void WorkerThread::wakeup(void)
{
    for(size_t id=0, max=state.size(); id < max; id ++)
    {
        state[id] = WAKEUP;
        event[id] = 0;
    }
    for(size_t id=0, max=tasks.size(); id < max; id ++)
    {
        thread temp(&WorkerThread::run, this, id);
        tasks[id].swap(temp);
    }
}

WorkerThread::~WorkerThread(void)
{
    for(size_t id=0, max=state.size(); id < max; id++)
    {
        {
            lock_guard<mutex> lock(mtx[id]);
            state[id] = END;
        }
        cond[id].notify_one();
    }
    for(auto & obj : tasks)
    {
        obj.join();
    }
}

size_t WorkerThread::getWaitTask(void)
{
    size_t result;
    auto lamda = [this, &result]
    {
        for(size_t id=0, max=state.size(); id < max; id++)
        {
            lock_guard<mutex> lock(mtx[id]);
            if(state[id] == WAIT)
            {
                result = id;
                return true;
            }
        }
        return false;
    };
    unique_lock<mutex> lock(master_mtx);
    master_cond.wait(lock, lamda);
    return result;
}

void WorkerThread::waitEmptyEvent(size_t id, unsigned int event)
{
    auto lamda = [this, id, event]
    {
        if(this->event[id] & event)
        {
            return false;
        }
        return true;
    };
    unique_lock<mutex> lock(mtx[id]);
    master_cond.wait(lock, lamda);
}

void WorkerThread::waitEmptyEvent(void)
{
    auto lamda = [this]
    {
        for(size_t id=0, max=state.size(); id < max; id++)
        {
            lock_guard<mutex> lock(mtx[id]);
            if(RUN == state[id])
            {
                return false;
            }
            if(0 != event[id])
            {
                return false;
            }
        }
        return true;
    };
    unique_lock<mutex> lock(master_mtx);
    master_cond.wait(lock, lamda);
}

void WorkerThread::setEvent(size_t id, unsigned int event)
{
    if(0 != event)
    {
        {
            lock_guard<mutex> lock(mtx[id]);
            if(END == state[id])
            {
                return;
            }
            state[id] = RUN;
            this->event[id] |= event;
        }
        cond[id].notify_one();
    }
}

unsigned int WorkerThread::wait(size_t id, unsigned int wait_event)
{
    unsigned int event = 0;
    auto lamda = [this, id, &event, wait_event]
    {
        if(END == state[id])
        {
            event = 0;
            return true;
        }
        event = this->event[id] & wait_event;
        this->event[id] &= ~wait_event;
        if(0 != event)
        {
            state[id] = RUN;
            return true;
        }
        state[id] = RUN_WAIT;
        master_cond.notify_one();
        return false;
    };
    unique_lock<mutex> lock(mtx[id]);
    cond[id].wait(lock, lamda);
    master_cond.notify_one();
    return event;
}

int WorkerThread::refState(size_t id)
{
    lock_guard<mutex> lock(mtx[id]);
    return state[id];
}

void WorkerThread::run(size_t id)
{
    unsigned int event = 0;
    while(END != refState(id))
    {
        master_cond.notify_one();
        {
            auto lamda = [this, id, &event]
            {
                if(END == state[id])
                {
                    event = 0;
                    return true;
                }
                event |= this->event[id];
                this->event[id] = 0;
                if(0 != event)
                {
                    state[id] = RUN;
                    return true;
                }
                state[id] = WAIT;
                master_cond.notify_one();
                return false;
            };
            unique_lock<mutex> lock(mtx[id]);
            cond[id].wait(lock, lamda);
            master_cond.notify_one();
        }
        for(unsigned int evt=1; 0 != evt; evt = evt << 1)
        {
            if(event & evt)
            {
                main(id, evt);
                event &= ~evt;
                break;
            }
        }
    }
}

void WorkerThread::main(size_t id, unsigned int event)
{
}

#ifdef _TEST_WORKER_THREAD
#include <iostream>
class TestThread : public WorkerThread
{
public:
    TestThread(void);
    virtual ~TestThread(void);
    virtual void main(size_t id, unsigned int event);
    void operator() (void);
};

TestThread::TestThread(void)
//  : WorkerThread(8)
{
}

TestThread::~TestThread(void)
{
}

void TestThread ::main(size_t id, unsigned int event)
{
    printf("  ID(%d): %08x\n", static_cast<unsigned int>(id), event);
    if(0xf & event)
    {
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

void TestThread::operator() (void)
{
    for(size_t cnt=0; cnt<10; cnt ++)
    {
        static const size_t list[] = { 0x01, 0x04, 0x18, 0x10, 0x20, 0x40, 0x80 };
        for(auto event: list)
        {
            size_t id = getWaitTask();
            setEvent(id, event);
        }
        waitEmptyEvent();
    }
}

int main(int argc, char * argv[])
{
    try
    {
        cout << "create" << endl;
        TestThread tes;
        cout << "start" << endl;
        tes();
        cout << "end" << endl;
    }
    catch(exception & exp)
    {
        cout << exp.what() << endl;
    }
    catch(...)
    {
        cout << "Ohter exceptions" << endl;
    }
    fflush(stdout);
    return 0;
}
#endif
