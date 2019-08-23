#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <string>
#include <regex>
#include <fstream>
#include <iostream>

using namespace std;
using namespace boost::program_options;

class SerialConrol
{
protected:
    boost::asio::io_service     io;
    boost::asio::serial_port    port;
    string                      fname;

public:
    SerialConrol(const string & name);
    virtual ~SerialConrol(void);
    virtual size_t send(const char * data, size_t size);
    virtual size_t send(const string & text);
    virtual void wait(const string & text);
};

SerialConrol::SerialConrol(const string & name)
  : port(io, name)//, is(&buff)
{
    port.set_option(boost::asio::serial_port_base::baud_rate(9600));
    port.set_option(boost::asio::serial_port_base::character_size(8));
    port.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
    port.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::even));
    port.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
}

SerialConrol::~SerialConrol(void)
{
    port.close();
}

size_t SerialConrol::send(const char * data, size_t size)
{
    if(0<size)
    {
        if(port.is_open())
        {
            size_t len = port.write_some(boost::asio::buffer(data, size));
            return len;
        }
    }
    return 0;
}

size_t SerialConrol::send(const string & text)
{
    return send(text.c_str(), text.size());
}

void SerialConrol::wait(const string & text)
{
    if(port.is_open())
    {
        string str;
        regex re(text);
        while(1)
        {
            boost::asio::streambuf      buff;
            boost::asio::read_until(port, buff, "\n");
            istream                     is(&buff);
            while(getline(is, str))
            {
                cout << str << endl;
                if(regex_match(str, re))
                {
                    return;
                }
            }
        }
    }
}

#ifdef _TEST_SERIAL
int main(int argc, char * argv[])
{
    try
    {
        string dev_name;
        options_description desc("Commandline options");
        desc.add_options()
            ("help,h",                          "help")
            ("com,c",       value<string>(),    "serial comunication device file");
        variables_map argmap;
        store(parse_command_line(argc, argv, desc), argmap);
        notify(argmap);
        if(argmap.count("help"))
        {
            cout << desc << endl;
            cout.flush();
            return 0;
        }
        if(argmap.count("com"))
        {
            dev_name = argmap["com"].as<string>();
        }
        else
        {
            cout << "must of cominication option" << endl;
            cout << desc << endl;
            cout.flush();
            return -1;
        }
        SerialConrol com(dev_name);
        com.send("test");
        //com.wait("OK");
        com.wait("#endif");
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
