#include "WorkerThread.cpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdio>

using namespace std;

class TextFilter : public WorkerThread
{
public:
    enum
    {
        RCV_END = 0x80000000,
    };
protected:
    size_t tid[4];

public:
    TextFilter(void);
    virtual ~TextFilter(void);
    virtual void main(size_t id, unsigned int evt);
    virtual void operator () (void);
};
TextFilter::TextFilter(void)
{
}
TextFilter::~TextFilter(void)
{
}
void TextFilter::main(size_t id, unsigned int evt)
{
}
void TextFilter::operator () (void)
{
    string str;
    while(getline(cin, str))
    {
        cout << str << endl;
    }
}

#ifdef _TEST_TEXT_FILTER
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

using namespace boost::program_options;

int main(int argc, char * argv[])
{
    try
    {
        options_description desc("Commandline options");
        desc.add_options()
            ("file,f",  value<string>(),    "file")
            ("help,h",                      "help");
        variables_map argmap;
        store(parse_command_line(argc, argv, desc), argmap);
        notify(argmap);
        if(argmap.count("help"))
        {
            cout << desc << endl;
            cout.flush();
            return 0;
        }
        string fname;
        if(argmap.count("file"))
        {
            fname = argmap["file"].as<string>();
        }
//        TextFilter app;
//        app();
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
#endif
