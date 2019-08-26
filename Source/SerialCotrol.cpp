/**
 * SerialControl Template on boost
 *
 * @file    SerialContol.cpp
 * @brief   Serial Control Sample Code on Boost Library and std::istream
 * @author  Shouji, Igarashi
 *
 * (c) 2018 Shouji, Igarashi.
 *
 * <li>Sample Test
 * <pre>
 *  # socat -d -d pty,raw,echo=0 pty,raw,echo=0
 *  2019/08/26 10:30:11 socat[1921] N PTY is /dev/pts/2
 *  2019/08/26 10:30:11 socat[1921] N PTY is /dev/pts/3
 *  # SerialCotrol.exe -c /dev/pts/2
 *  # cat Source/SerialCotrol.cpp > /dev/pts/3
 * </pre>
 *
 * @see string
 * @see istream
 * @see boost::asio::streambuf
 * @see boost::asio::io_service
 * @see boost::asio::serial_port
 */

#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <string>
#include <regex>
#include <fstream>
#include <iostream>

using namespace std;
using namespace boost::program_options;

/**
 * Serial Control on Boost library and std::istream
 * default serial profile 
 * item            | descriptor
 * ----------------|------------
 *  Speed          | 9600 bps
 *  character size | 8 bit
 *  stop bit       | 1 bit
 *  parity         | even
 *
 */
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

/**
 * constractor on SerialControl
 *
 * @param name      serial device file
 */
SerialConrol::SerialConrol(const string & name)
  : port(io, name)
{
    port.set_option(boost::asio::serial_port_base::baud_rate(9600));
    port.set_option(boost::asio::serial_port_base::character_size(8));
    port.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
    port.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::even));
    port.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
}

/**
 * destractor on SerialControl
 */
SerialConrol::~SerialConrol(void)
{
    port.close();
}

/**
 * data send of byte data
 *
 * @param   data        buffer pointer on send data
 * @param   size        buffer size on send data
 * @return  data size of the transmission result
 */
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

/**
 * data send of string
 *
 * @param   text        The string to send
 * @return  data size of the transmission result
 */
size_t SerialConrol::send(const string & text)
{
    return send(text.c_str(), text.size());
}

/**
 * waitting of string receive
 *
 * @param   text        receiving of string
 */
void SerialConrol::wait(const string & text)
{
    if(port.is_open())
    {
        string str;
        regex re(text);
        while(1)
        {
            boost::asio::streambuf      buff;
            istream                     is(&buff);
            boost::asio::read_until(port, buff, "\n");
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
/*
 * Test Code of Serial Control 
 * <pre>
 *  # socat -d -d pty,raw,echo=0 pty,raw,echo=0
 *  2019/08/26 10:30:11 socat[1921] N PTY is /dev/pts/2
 *  2019/08/26 10:30:11 socat[1921] N PTY is /dev/pts/3
 *  # SerialCotrol.exe -c /dev/pts/2
 *  # cat Source/SerialCotrol.cpp > /dev/pts/3
 * </pre>
 */
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
