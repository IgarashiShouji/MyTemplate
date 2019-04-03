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
        BEGIN = 0,
        WAKEUP,
        RUN,
        CNT = 4,
    };
    mutex               master_mtx;
    condition_variable  master_cond;
    mutex               mtx[CNT];
    condition_variable  cond[CNT];
    thread              th[CNT];
    int                 state[CNT];
    void (*func)(MyThread & self, int idx);
    unsigned int        event[CNT];
public:
    MyThread(void);
    virtual ~MyThread(void);
    void main(int idx);
    void wakeup(int idx, void (*func)(MyThread & self, int idx));
protected:
    void waitWakeup(int idx);
public:
    unsigned int wait(int idx);
    void setEvent(int idx, unsigned int event);
    void waitClearEvent(int idx, unsigned int event);
};
