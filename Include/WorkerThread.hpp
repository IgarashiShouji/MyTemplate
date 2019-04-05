#ifndef __WorkerThread_hpp__
#define __WorkerThread_hpp__

#include <thread>
#include <mutex>
#include <condition_variable>

class WorkerThread
{
public:
    enum
    {
        WAKEUP = 0,
        WAIT,
        RUN,
        ID_MAX = 4,
    };
private:
    std::mutex              master_mtx;
    std::condition_variable master_cond;
    std::mutex              mtx[ID_MAX];
    std::condition_variable cond[ID_MAX];
    std::thread             tasks[ID_MAX];
    int                     state[ID_MAX];
    unsigned int            event[ID_MAX];
public:
    WorkerThread(void);
    virtual ~WorkerThread(void);
    size_t getWaitTask(void);
    void setEvent(size_t id, unsigned int event);
    void waitClearEvent(size_t id, unsigned int event);
    void waitClearEventAll(void);
    unsigned int wait(size_t id, unsigned int wait_event);
    virtual void main(size_t id, unsigned int event);

protected:
    void run(size_t id);
};

#endif
