#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <cstdio>

using namespace std;

class MyThread
{
private:
    enum
    {
        WAKEUP = 0,
        WAIT,
        RUN,
    };
    mutex                       master_mtx;
    condition_variable          master_cond;
    vector<mutex>               mtx;
    vector<condition_variable>  cond;
    vector<thread>              task;
    vector<int>                 state;
    vector<unsigned int>        event;
public:
    MyThread(void);
    virtual ~MyThread(void);

    void main(int idx, unsigned int event);
    void setEvent(int idx, unsigned int event);
    void waitClearEvent(int idx, unsigned int event);
    unsigned int waitEvent(unsigned int mask);
};
