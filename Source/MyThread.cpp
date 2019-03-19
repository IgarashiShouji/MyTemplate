#include "MyThread.hpp"

MyThread::MyThread(void)
{
    /* init state & valiables */
    for(int idx=0, max=CNT; idx<max; idx ++)
    {
        state[idx]  = BEGIN;
        event[idx] = 0;
        func       = nullptr;
    }
    /* start thread */
    for(int idx=0, max=CNT; idx<max; idx ++)
    {
        thread temp(&MyThread::main, this, idx);
        th[idx].swap(temp);
    }
}

MyThread::~MyThread(void)
{
    for(auto & obj : th)
    {
        obj.join();
    }
}

void MyThread::wakeup(int idx, void (*func)(MyThread & self, int idx))
{
    {
        lock_guard<mutex> lock(mtx[idx]);
        state[idx] = WAKEUP;
        this->func = func;
    }
    cond[idx].notify_one();
}

void MyThread::waitWakeup(void)
{
    auto lamda = [this]
    {
        for(int idx=0, max=CNT; idx<max; idx++)
        {
            if(RUN != state[idx])
            {
                return false;
            }
        }
        return true;
    };
    unique_lock<mutex> lock(master_mtx);
    master_cond.wait(lock, lamda);
}

unsigned int MyThread::wait(int idx)
{
    unsigned int event = 0;
    {
        auto lamda = [this, idx]
        {
            bool result   = false;
            if(0 != this->event[idx])
            {
                result = true;
            }
            return result;
        };
        unique_lock<mutex> lock(mtx[idx]);
        cond[idx].wait(lock, lamda);
        event = this->event[idx];
        {
            lock_guard<mutex> lock(master_mtx);
            this->event[idx] = 0;
            master_cond.notify_one();
        }
    }
    return event;
}

void MyThread::setEvent(int idx, unsigned int event)
{
    {
        lock_guard<mutex> lock(mtx[idx]);
        this->event[idx] |= event;
    }
    cond[idx].notify_one();
}

void MyThread::waitClearEvent(int idx, unsigned int event)
{
    auto lamda = [this, idx, event]
    {
        if(this->event[idx] & ~event)
        {
            return false;
        }
        return true;
    };
    unique_lock<mutex> lock(master_mtx);
    master_cond.wait(lock, lamda);
}

void MyThread::main(int idx)
{
    {
        auto lamda = [this, idx]
        {
            if(WAKEUP == state[idx])
            {
                lock_guard<mutex> lock(master_mtx);
                state[idx] = RUN;
                master_cond.notify_one();
                return true;
            }
            return false;
        };
        unique_lock<mutex> lock(mtx[idx]);
        cond[idx].wait(lock, lamda);
    }
    if(nullptr != func)
    {
        (func)(*this, idx);
    }
}
