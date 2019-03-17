#include "MyThread.cpp"

int main(int argc, char * argv[])
{
    try
    {
        MyThread myThread;
        static const int list[4] = { 3, 1, 2, 0 };
        for(auto idx: list)
        {
            myThread.wakeup(idx);
            this_thread::sleep_for(chrono::milliseconds(500));
            //this_thread::sleep_for(chrono::seconds(1));
        }
    }
    catch(exception & exp)
    {
    }
    return 0;
}
