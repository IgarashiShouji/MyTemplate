#include "MyThread.cpp"

static void test(MyThread & the, int idx)
{
    printf("スレッド %d\n", idx);
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


int main(int argc, char * argv[])
{
    try
    {
        MyThread myThread;
        static const int list[4] = { 3, 1, 2, 0 };
        for(auto idx: list)
        {
            static void (* const tblFunction[4])(MyThread & the, int idx) = { test, test, test, test };
            myThread.wakeup(idx, tblFunction[idx]);
        }
        myThread.waitWakeup();
        for(auto idx: list)
        {
            myThread.setEvent(idx, 2);
            //myThread.waitClearEvent(idx, 2);
        }
        for(auto idx: list)
        {
            myThread.setEvent(idx, 1);
            myThread.waitClearEvent(idx, 1);
        }
    }
    catch(exception & exp)
    {
    }
    fflush(stdout);
    return 0;
}
