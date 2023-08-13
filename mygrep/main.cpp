#include <iostream>
#include <fstream>
#include <cstdio>

#include <sstream>
#include <string>
#include <regex>

#include <list>
#include <vector>
#include <map>

#include <filesystem>
#include <ctime>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include <boost/program_options.hpp>


class GrepThread
{
private:
    std::regex              reg;
    volatile bool           th_enable;
    GrepThread *            next;
    std::list<std::string>  cache;
    std::thread             th_ctrl;
    std::mutex              mtx;
    std::condition_variable cond;
    bool                    notify_state;
public:
    GrepThread(std::string & str) : reg(str),     th_enable(false),         next(nullptr),  cache(0), notify_state(0) { }
    GrepThread(GrepThread & obj)  : reg(obj.reg), th_enable(obj.th_enable), next(obj.next), cache(0), notify_state(0) { }
    inline void start(size_t id)
    {
        std::lock_guard<std::mutex> lock(mtx);
        th_enable = true;
        std::thread temp(&GrepThread::match, this, id);
        th_ctrl.swap(temp);
    }
    inline bool check(void)
    {
        std::lock_guard<std::mutex> lock(mtx);
        if((th_enable) || (0 < cache.size())) { return true; }
        return false;
    }
    inline void match(size_t id)
    {
        auto lamda = [this]
        {
            auto size = cache.size();
            if(0 < size)    { return true; }
            if(notify_state){ return true; }
            return false;
        };
        while(check())
        {
            std::unique_lock<std::mutex> lock(mtx);
            cond.wait(lock, lamda);
            notify_state = false;
            if(0 < cache.size())
            {
                auto str = cache.front();
                cache.pop_front();
                lock.unlock();
                if(search(str))
                {
                    if(nullptr != next) { next->push(str);               }
                    else                { std::cout << str << std::endl; }
                }
            }
            else
            {
                lock.unlock();
            }
        }
    }
    inline void set_next(GrepThread * p)  { next = p; }
    inline bool search(std::string & str) { return std::regex_search(str, reg); }
    inline void push(std::string & str)
    {
        std::lock_guard<std::mutex> lock(mtx);
        cache.push_back(str);
        notify_state = true;
        cond.notify_all();
    }
    inline void stop(void)
    {
        std::lock_guard<std::mutex> lock(mtx);
        th_enable = false;
        notify_state = true;
        cond.notify_all();
    }
    inline void join(void)
    {
        th_ctrl.join();
    }
    virtual ~GrepThread(void)
    {
        if(th_enable)
        {
            stop();
            join();
        }
    }
};

int main(int argc, char * argv[])
{
    try
    {
        boost::program_options::options_description desc("my-grep [Options] patterns");
        desc.add_options()
            ("mode,m", boost::program_options::value<std::string>(), "search mode --mode=[AND/OR] (default AND)")
            ("help,h",    "help");
        boost::program_options::variables_map argmap;
        auto const parsing_result = parse_command_line( argc, argv, desc );
        store( parsing_result, argmap );
        notify( argmap );
        std::vector<GrepThread *> regs;
        for(auto const & str : collect_unrecognized(parsing_result.options, boost::program_options::include_positional))
        {
            std::string test(str);
            GrepThread * reg = new GrepThread(test);
            regs.push_back(reg);
        }
        if(argmap.count("help") || 0 == regs.size())
        {
            std::cout << desc << std::endl;
            return 0;
        }
        std::string str;
        std::string mode;
        if(argmap.count("mode")) { mode = argmap["mode"].as<std::string>(); }
        if(mode == "OR")
        {
            while(std::getline(std::cin, str))
            {
                for(auto & reg: regs)
                {
                    if(reg->search(str))
                    {
                        std::cout << str << std::endl;
                        break;
                    }
                }
            }
        }
        else
        {
            for(size_t idx=0; idx < regs.size(); idx ++)
            {
                if((idx + 1) < regs.size()) { regs[idx]->set_next(regs[idx+1]); }
                (regs[idx])->start(idx);
            }
            while(std::getline(std::cin, str))
            {
                (regs[0])->push(str);
            }
            for(auto & reg: regs) { reg->stop(); reg->join(); };
        }
        for( auto & reg : regs ) { delete reg; }
        regs.clear();
    }
    catch(std::exception & exp) { std::cerr << "exeption: " << exp.what() << std::endl; }
    catch(...)                  { std::cerr << "unknown exeption" << std::endl;         }
    return 0;
}
