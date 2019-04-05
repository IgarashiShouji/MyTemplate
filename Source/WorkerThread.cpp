#include "WorkerThread.hpp"
#include <cstdio>

using namespace std;

WorkerThread::WorkerThread(void)
{
    for(size_t id=0; id<ID_MAX; id ++)
    {
        state[id] = WAKEUP;
        event[id] = 0;
    }
    for(size_t id=0; id<ID_MAX; id ++)
    {
        thread temp(&WorkerThread::run, this, id);
        tasks[id].swap(temp);
    }
    for(size_t id=0; id<ID_MAX; id++)
    {
        {
            {
                lock_guard<mutex> lock(mtx[id]);
            }
            cond[id].notify_one();
            auto lamda = [this, id]
            {
                if(WAKEUP != state[id])
                {
                    return true;
                }
                return false;
            };
            unique_lock<mutex> lock(master_mtx);
            master_cond.wait(lock, lamda);
        }
    }
}

WorkerThread::~WorkerThread(void)
{
    for(size_t id=0; id<ID_MAX; id++)
    {
        {
            {
                lock_guard<mutex> lock(mtx[id]);
                this->state[id] = RUN;
                this->event[id] |= 1;
            }
            cond[id].notify_one();
        }
        waitClearEvent(id, 1);
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
        for(size_t id=0; id<ID_MAX; id++)
        {
            if(this->state[id] == WAIT)
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

void WorkerThread::setEvent(size_t id, unsigned int event)
{
    event &= ~1;
    if(0 != event)
    {
        {
            lock_guard<mutex> lock(mtx[id]);
            this->state[id] = RUN;
            this->event[id] |= event;
        }
        cond[id].notify_one();
    }
}

void WorkerThread::waitClearEvent(size_t id, unsigned int event)
{
    auto lamda = [this, id, event]
    {
        if(this->event[id] & event)
        {
            return false;
        }
        return true;
    };
    unique_lock<mutex> lock(master_mtx);
    master_cond.wait(lock, lamda);
}

void WorkerThread::waitClearEventAll(void)
{
    auto lamda = [this]
    {
        for(size_t id=0; id<ID_MAX; id++)
        {
            if(this->event[id] & ~1)
            {
                return false;
            }
        }
        return true;
    };
    unique_lock<mutex> lock(master_mtx);
    master_cond.wait(lock, lamda);
}

void WorkerThread::run(size_t id)
{
    {
        auto lamda = [this, id]
        {
            if(WAKEUP == state[id])
            {
                lock_guard<mutex> lock(master_mtx);
                this->state[id] = WAIT;
                master_cond.notify_one();
                return true;
            }
            return false;
        };
        unique_lock<mutex> lock(mtx[id]);
        cond[id].wait(lock, lamda);
    }
    unsigned int event = 0;
    while(1)
    {
        {
            lock_guard<mutex> lock_master(master_mtx);
            this->state[id] = WAIT;
        }
        master_cond.notify_one();
        {
            auto lamda = [this, id]
            {
                if(0 != this->event[id])
                {
                    return true;
                }
                return false;
            };
            unique_lock<mutex> lock(mtx[id]);
            cond[id].wait(lock, lamda);
            lock_guard<mutex> lock_master(master_mtx);
            event |= this->event[id];
            this->event[id] = 0;
        }
        master_cond.notify_one();
        if(1 & event)
        {
            break;
        }
        event &= ~1;
        for(unsigned int evt=2; 0 != evt; evt = evt << 1)
        {
            if(event & evt)
            {
                main(id, evt);
                event &= ~evt;
            }
        }
    }
}
void WorkerThread::main(size_t id, unsigned int event)
{
}

#ifdef _TEST_MAIN
#include <iostream>

class TestThread : public WorkerThread
{
public:
    TestThread(void) {}
    virtual ~TestThread(void){}
    virtual void main(size_t id, unsigned int event);
};
void TestThread::main(size_t id, unsigned int event)
{
    printf("id = %x: event = %08x\n", static_cast<int>(id), event);
    fflush(stdout);
    if(event & 0xf0)
    {
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

int main(int argc, char * argv[])
{
    try
    {
        TestThread myThread;
        static const size_t list[] = { 0x02, 0x04, 0x18, 0x10, 0x20, 0x40, 0x80 };
        for(auto evt: list)
        {
            size_t id = myThread.getWaitTask();
            myThread.setEvent(id, evt);
        }
        myThread.waitClearEventAll();
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
