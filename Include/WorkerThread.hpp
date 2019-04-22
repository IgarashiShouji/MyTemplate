#ifndef __WorkerThread_hpp__
#define __WorkerThread_hpp__

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

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
    std::mutex                              master_mtx;
    std::condition_variable                 master_cond;
    std::vector<std::thread>                tasks;
    std::vector<std::mutex>                 mtx;
    std::vector<std::condition_variable>    cond;
    std::vector<int>                        state;
    std::vector<unsigned int>               event;
public:
    WorkerThread(void);
    WorkerThread(int th_cnt);
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

private:
    void wakeup(void);
};

#endif
