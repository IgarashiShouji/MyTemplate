#include <boost/process.hpp>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <regex>
#include <string>
#include <iostream>
#include <cstring>
#include <cstdio>

using namespace boost::process;
using namespace std;

class ChildProcess
{
private:
    string      line;
    ipstream    pipe_out;
    ipstream    pipe_err;
    opstream *  pipe_in;
    child *     ps;
public:
    ChildProcess(const char * cmd);
    ChildProcess(const string & cmd);
    virtual ~ChildProcess(void);
    virtual void closeInput(void);
    opstream & in(void);
    void wait(void);
    void wait(const string & key);
    void wait(const char * key);
};

ChildProcess::ChildProcess(const char * cmd)
{
    pipe_in = new opstream();
    ps = new child(cmd, std_out > pipe_out, std_err > pipe_err, std_in < *pipe_in);
}

ChildProcess::ChildProcess(const string & cmd)
{
    pipe_in = new opstream();
    ps = new child(cmd, std_out > pipe_out, std_err > pipe_err, std_in < *pipe_in);
}

ChildProcess::~ChildProcess(void)
{
    closeInput();
    delete ps;
    ps = nullptr;
}

void ChildProcess::closeInput(void)
{
    if(nullptr != pipe_in)
    {
        delete pipe_in;
        pipe_in = nullptr;
    }
}

opstream & ChildProcess::in(void)
{
    return *pipe_in;
}

void ChildProcess::wait(const char * key)
{
    string str(key);
    wait(str);
}

void ChildProcess::wait(const string & key)
{
    regex reg(key);
    while(pipe_out && getline(pipe_out, line))
    {
        cout << line << endl;
        if(regex_search(line, reg))
        {
            break;
        }
        ps->wait();
    }
}

void ChildProcess::wait(void)
{
    while(pipe_out && getline(pipe_out, line))
    {
        cout << line << endl;
        ps->wait();
    }
    while(pipe_err && getline(pipe_err, line))
    {
        cout << line << endl;
        ps->wait();
    }
}

int main(int argc, char * argv[])
{
    try
    {
        string key("string 2");
        ChildProcess ps("cat");
        (ps.in()) << "test string 1" << endl;
        (ps.in()) << "test " << key << endl;
        (ps.in()) << "test string 3" << endl;
        ps.closeInput();
        ps.wait(key);
        cout << "wait 2" << endl;
        ps.wait();
    }
    catch(exception & exp)
    {
        cout << exp.what() << endl;
    }
    catch(...)
    {
        cout << "Ohter exceptions" << endl;
    }
    cout.flush();
    return 0;
}