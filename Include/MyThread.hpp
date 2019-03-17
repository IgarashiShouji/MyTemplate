#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstdio>

using namespace std;

class MyThread
{
private:
    enum
    {
        CNT = 4
    };
    mutex               mtx[CNT];
    condition_variable  cond[CNT];
    thread              th[CNT];
    bool                is_ready[CNT];
public:
    MyThread(void);
    virtual ~MyThread(void);
    void main(int idx);
    void wakeup(int idx);
};
