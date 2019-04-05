#include "WorkerThread.cpp"

#include <boost/asio.hpp>
#include <iostream>
#include <cstring>
#include <cstdio>

using namespace std;

class SerialConrol : public WorkerThread
{
public:
    enum
    {
        EXEC_TX         = 0x00000002,
        EXEC_RX         = 0x00000004,
        RCV             = 0x00000040,
        RCV_END         = 0x00000080,
    };
protected:
    boost::asio::io_service     io;
    boost::asio::serial_port    port;
    vector<string>              req_list;
    unsigned int                req_idx;
    size_t                      TxID;
    size_t                      RxID;

public:
    SerialConrol(const char * name);
    virtual ~SerialConrol(void);
    virtual size_t send(const char * data, size_t size);
    virtual size_t recive(unsigned char * data, size_t size);
    virtual void close(void);
    virtual void main(size_t id, unsigned int evt);


    virtual void operator () (void);
    virtual void execTx(void);
    virtual void execRx(void);
};
SerialConrol::SerialConrol(const char * name)
  : port(io, name)
{
    port.set_option(boost::asio::serial_port_base::baud_rate(9600));
    port.set_option(boost::asio::serial_port_base::character_size(8));
    port.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
    port.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::even));
    port.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
}
SerialConrol::~SerialConrol(void)
{
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
size_t SerialConrol::recive(unsigned char * data, size_t size)
{
   if(port.is_open())
   {
       size_t len = port.read_some(boost::asio::buffer(data, size));
       return len;
   }
   return 0;
}
void SerialConrol::close(void)
{
    port.close();
}
static unsigned char calcSum(char * buff, size_t size)
{
    unsigned char sum = 0;
    for(size_t idx=0; idx < size; idx++)
    {
        sum += static_cast<unsigned char>(buff[idx]);
    }
    return sum;
}
void SerialConrol::main(size_t id, unsigned int evt)
{
    switch(evt)
    {
    case EXEC_TX:
        execTx();
        break;
    case EXEC_RX:
        execRx();
        break;
    default:
        break;
    }
}
void SerialConrol::execTx(void)
{
    char buff[256];
    RxID = getWaitTask();
    setEvent(RxID, EXEC_RX);
    if(RCV_END & wait(TxID, RCV|RCV_END))
    {
        cout.flush();
        return;
    }
    static const char * const cmd[] { "Start", "End" };
    cout << cmd[0] << ":";
    sprintf(buff, "%s\x0d\x0a", cmd[0]);
    send(buff, strlen(buff));
    cout.flush();
    for(unsigned short addr=0x0000; addr < 0x2000; addr += 8)
    {
        sprintf(buff, "0x%04X", addr);
        unsigned char sum = calcSum(buff, strlen(buff));
        sprintf(buff, "0x%04X%02X\x0d\x0a", addr, sum);
        if(RCV_END & wait(TxID, RCV|RCV_END))
        {
            cout.flush();
            return;
        }
        printf("0x%04X%02X:", addr, sum);
        send(&(buff[0]), strlen(&(buff[0])));
        req_list.push_back(buff);
    }
    if(!(RCV_END & wait(TxID, RCV|RCV_END)))
    {
        cout << cmd[1] << ":";
        sprintf(buff, "%s\x0d\x0a", cmd[1]);
        send(buff, strlen(buff));
    }
    cout.flush();
}
void SerialConrol::execRx(void)
{
    setEvent(TxID, RCV);

    string str;
    boost::asio::streambuf buff;
    istream is(&buff);

    boost::asio::read_until(port, buff, "\n");
    getline(is, str);
    if(0 < str.size())
    {
        str.erase(str.size()-1,1);
        cout  << str << endl;
        cout.flush();
        setEvent(TxID, RCV);
    }
    str.clear();
    boost::asio::read_until(port, buff, "\n");
    while(getline(is, str))
    {
        if(0 < str.size())
        {
            str.erase(str.size()-1,1);
            cout  << str << endl;
            cout.flush();
            setEvent(TxID, RCV);
        }
        if(str == "END")
        {
            break;
        }
        str.clear();
        boost::asio::read_until(port, buff, "\n");
    }
    setEvent(TxID, RCV_END);
    cout.flush();
}
void SerialConrol::operator () (void)
{
    TxID = getWaitTask();
    setEvent(TxID, EXEC_TX);
    waitClearEventAll();
    cout.flush();
}

int main(int argc, char * argv[])
{
    try
    {
        SerialConrol com(argv[1]);
        com();
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
