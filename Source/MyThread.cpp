#include "MyThread.hpp"

MyThread::MyThread(void)
//  : th { thread(&MyThread::main, this, 0), thread(&MyThread::main, this, 1), thread(&MyThread::main, this, 2), thread(&MyThread::main, this, 3) }
{
    for(int idx=0, max=CNT; idx<max; idx ++)
    {
        is_ready[idx] = false;
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

void MyThread::wakeup(int idx)
{
    {
        lock_guard<mutex> lock(mtx[idx]);
        is_ready[idx] = true;
    }
    cond[idx].notify_one();
}

void MyThread::main(int idx)
{
    {
        unique_lock<mutex> lock(mtx[idx]);
        printf("スレッド %d\n", idx);
        cond[idx].wait(lock, [this, idx] { bool result=is_ready[idx]; is_ready[idx]=false; return result; });
    }
    printf("スレッド %d End\n", idx);
}
