#include "MyThread.cpp"
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
