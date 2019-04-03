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
    waitWakeup(idx);
}

void MyThread::waitWakeup(int idx)
{
    auto lamda = [this, idx]
    {
        if(RUN != state[idx])
        {
            return false;
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
    void (*func)(MyThread & self, int idx) = nullptr;
    {
        auto lamda = [this, idx, &func]
        {
            if(WAKEUP == state[idx])
            {
                lock_guard<mutex> lock(master_mtx);
                state[idx] = RUN;
                func = this->func;
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

#ifdef _TEST_MAIN
#include <iostream>

static void test(MyThread & the, int idx)
{
    while(1)
    {
        unsigned int event = the.wait(idx);
        printf("スレッド %d: %08x\n", idx, event);
        if(1 & event)
        {
            break;
        }
    }
    printf("スレッド %d End\n", idx);
}
static void test1(MyThread & the, int idx)
{
    printf("test1 %d\n", idx);
    test(the, idx);
}

static void test2(MyThread & the, int idx)
{
    printf("test2 %d\n", idx);
    test(the, idx);
}

static void test3(MyThread & the, int idx)
{
    printf("test3 %d\n", idx);
    test(the, idx);
}

static void test4(MyThread & the, int idx)
{
    printf("test4 %d\n", idx);
    test(the, idx);
}

int main(int argc, char * argv[])
{
    try
    {
        MyThread myThread;
        static const int list[4] = { 3, 1, 2, 0 };
        for(int idx=0; idx<4; idx++)
        {
            static void (* const tblFunction[4])(MyThread & the, int idx) = { test1, test2, test3, test4 };
            myThread.wakeup(idx, tblFunction[idx]);
        }
        for(auto idx: list)
        {
            myThread.setEvent(idx, 2);
        }
        for(auto idx: list)
        {
            myThread.setEvent(idx, 1);
            myThread.waitClearEvent(idx, 1);
        }
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
