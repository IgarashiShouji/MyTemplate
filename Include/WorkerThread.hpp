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
        WAKEUP = 0,     // Thread State: State before transitioning to the first wait
        WAIT,           // Thread State: State of wait
        RUN,            // Thread State: State of run
        RUN_WAIT,       // Thread State: State of event wait on run
        END,            // Thread State: State of end
        ID_MAX = 4      // Thread Count
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
    void waitEmptyEvent(size_t id, unsigned int event);
    void waitEmptyEvent(void);
    void setEvent(size_t id, unsigned int event);
    unsigned int wait(size_t id, unsigned int wait_event);
    virtual void main(size_t id, unsigned int event);

protected:
    int refState(size_t id);
    void run(size_t id);
};

#endif
