#include "TblPresensDO.hpp"
#include "Entity.hpp"
#include <cstdio>
#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <boost/process/system.hpp>
#include <boost/process/io.hpp>
#include <boost/process/pipe.hpp>

using namespace std;
using namespace boost::process;

using namespace std;
using namespace MyEntity;
using namespace MyUtilities;

static const char * str_types[7] =
{
    "uint32",
    "int32",
    "float",
    "uint16",
    "int16",
    "uint8",
    "int8"
};

static const unsigned short modbusCRC[256] =
{
    0x0000, 0xC1C0, 0x81C1, 0x4001, 0x01C3, 0xC003, 0x8002, 0x41C2, 0x01C6, 0xC006, 0x8007, 0x41C7, 0x0005, 0xC1C5, 0x81C4,
    0x4004, 0x01CC, 0xC00C, 0x800D, 0x41CD, 0x000F, 0xC1CF, 0x81CE, 0x400E, 0x000A, 0xC1CA, 0x81CB, 0x400B, 0x01C9, 0xC009,
    0x8008, 0x41C8, 0x01D8, 0xC018, 0x8019, 0x41D9, 0x001B, 0xC1DB, 0x81DA, 0x401A, 0x001E, 0xC1DE, 0x81DF, 0x401F, 0x01DD,
    0xC01D, 0x801C, 0x41DC, 0x0014, 0xC1D4, 0x81D5, 0x4015, 0x01D7, 0xC017, 0x8016, 0x41D6, 0x01D2, 0xC012, 0x8013, 0x41D3,
    0x0011, 0xC1D1, 0x81D0, 0x4010, 0x01F0, 0xC030, 0x8031, 0x41F1, 0x0033, 0xC1F3, 0x81F2, 0x4032, 0x0036, 0xC1F6, 0x81F7,
    0x4037, 0x01F5, 0xC035, 0x8034, 0x41F4, 0x003C, 0xC1FC, 0x81FD, 0x403D, 0x01FF, 0xC03F, 0x803E, 0x41FE, 0x01FA, 0xC03A,
    0x803B, 0x41FB, 0x0039, 0xC1F9, 0x81F8, 0x4038, 0x0028, 0xC1E8, 0x81E9, 0x4029, 0x01EB, 0xC02B, 0x802A, 0x41EA, 0x01EE,
    0xC02E, 0x802F, 0x41EF, 0x002D, 0xC1ED, 0x81EC, 0x402C, 0x01E4, 0xC024, 0x8025, 0x41E5, 0x0027, 0xC1E7, 0x81E6, 0x4026,
    0x0022, 0xC1E2, 0x81E3, 0x4023, 0x01E1, 0xC021, 0x8020, 0x41E0, 0x01A0, 0xC060, 0x8061, 0x41A1, 0x0063, 0xC1A3, 0x81A2,
    0x4062, 0x0066, 0xC1A6, 0x81A7, 0x4067, 0x01A5, 0xC065, 0x8064, 0x41A4, 0x006C, 0xC1AC, 0x81AD, 0x406D, 0x01AF, 0xC06F,
    0x806E, 0x41AE, 0x01AA, 0xC06A, 0x806B, 0x41AB, 0x0069, 0xC1A9, 0x81A8, 0x4068, 0x0078, 0xC1B8, 0x81B9, 0x4079, 0x01BB,
    0xC07B, 0x807A, 0x41BA, 0x01BE, 0xC07E, 0x807F, 0x41BF, 0x007D, 0xC1BD, 0x81BC, 0x407C, 0x01B4, 0xC074, 0x8075, 0x41B5,
    0x0077, 0xC1B7, 0x81B6, 0x4076, 0x0072, 0xC1B2, 0x81B3, 0x4073, 0x01B1, 0xC071, 0x8070, 0x41B0, 0x0050, 0xC190, 0x8191,
    0x4051, 0x0193, 0xC053, 0x8052, 0x4192, 0x0196, 0xC056, 0x8057, 0x4197, 0x0055, 0xC195, 0x8194, 0x4054, 0x019C, 0xC05C,
    0x805D, 0x419D, 0x005F, 0xC19F, 0x819E, 0x405E, 0x005A, 0xC19A, 0x819B, 0x405B, 0x0199, 0xC059, 0x8058, 0x4198, 0x0188,
    0xC048, 0x8049, 0x4189, 0x004B, 0xC18B, 0x818A, 0x404A, 0x004E, 0xC18E, 0x818F, 0x404F, 0x018D, 0xC04D, 0x804C, 0x418C,
    0x0044, 0xC184, 0x8185, 0x4045, 0x0187, 0xC047, 0x8046, 0x4186, 0x0182, 0xC042, 0x8043, 0x4183, 0x0041, 0xC181, 0x8180,
    0x4040
};

/**
 * calclate CRC for modbus
 *
 * @param   crc     calclate result of crc on modbus
 * @param   data    hex string
 */
static void calcModbusCRC(string & crc, string & data)
{
    if((data.size()%2) == 0)
    {
        CalcCRC16 calc(modbusCRC);
        unsigned int size = 0;
        for(unsigned int idx=0, max=data.size();idx<max; idx += 2)
        {
            stringstream ss;
            ss << hex << data.substr(idx, 2);
            int val;
            ss >> val;
            calc << val;
            size ++;
        }
        char buff[16];
        sprintf(buff, "%04X", *calc);
        crc = buff;
    }
}

/**
 * get Data Type from data id
 *
 * @param id        data id
 * @return data type (0:uint32, 1:int32, 2:float, 3:uint16, 4:int16, 5:uint8, 6:int8, 7:unknown id)
 */
inline unsigned char getType(size_t id)
{
    if(id < (uint32_cnt + int32_cnt + float_cnt))
    {   /* uint32, int32, float */
        if(id < (uint32_cnt + int32_cnt))
        {   /* uint32, int32 */
            if(id < uint32_cnt)
            {   /* uint32 */
                return 0;
            }
            else
            {   /* int32 */
                return 1;
            }
        }
        else
        {   /* float */
            return 2;
        }
    }
    else
    {   /* uint16, int16, uint8, int8 */
        if(id < (uint32_cnt + int32_cnt + float_cnt + uint16_cnt + int16_cnt))
        {   /* uint16, int16 */
            if(id < (uint32_cnt + int32_cnt + float_cnt + uint16_cnt))
            {   /* uint16 */
                return 3;
            }
            else
            {   /* int16 */
                return 4;
            }
        }
        else
        {   /* uint8, int8 */
            if(id < (uint32_cnt + int32_cnt + float_cnt + uint16_cnt + int16_cnt + uint8_cnt))
            {   /* uint8 */
                return 5;
            }
            else
            {
                if(id < (uint32_cnt + int32_cnt + float_cnt + uint16_cnt + int16_cnt + uint8_cnt + int8_cnt))
                {   /* int8 */
                    return 6;
                }
            }
        }
    }
    return 7;   /* unknown id */
}

class ProcessControl
{
public:
    enum Event
    {
        EVT_NOEVENT,
        EVT_Gap,
        EVT_TimeOut1,
        EVT_TimeOut2,
        EVT_TimeOut3
    };
private:
    boost::process::opstream    ps_in;
    boost::process::ipstream    ps_out;
    boost::process::ipstream    ps_err;
    boost::process::child       ps;
    std::mutex                  mtx;
    std::condition_variable     cond;
    std::vector<std::thread>    tasks;
    enum Event                  event;
    string                      res;
public:
    ProcessControl(const char * cmd);
    virtual ~ProcessControl(void);
    string & response(void);
    void clear(void);
    void in(string cmdline);
    void wakeup(enum Event evt);
    enum Event wait(void);
protected:
    virtual void main(size_t id);
    virtual void watch_stdout(void);
    virtual void watch_stderr(void);
};
ProcessControl::ProcessControl(const char * cmd)
  : ps(cmd, boost::process::std_in < ps_in, boost::process::std_out > ps_out, boost::process::std_err > ps_err), tasks(2), event(EVT_NOEVENT), res("")
{
    for(size_t id=0, max=tasks.size(); id < max; id ++)
    {
        thread temp(&ProcessControl::main, this, id);
        tasks[id].swap(temp);
    }
}
ProcessControl::~ProcessControl(void)
{
    for(auto & th: tasks)
    {
        th.join();
    }
    ps_in.flush();
    ps_in.pipe().close();
    ps.wait();
}
string & ProcessControl::response(void)
{
    return res;
}
void ProcessControl::clear(void)
{
    res.clear();
}
void ProcessControl::in(string cmdline)
{
    res.clear();
    ps_in << cmdline << endl;
}
void ProcessControl::wakeup(enum Event evt)
{
    lock_guard<mutex> lock(mtx);
    event = evt;
    cond.notify_one();
}
enum ProcessControl::Event ProcessControl::wait(void)
{
    enum ProcessControl::Event result;
    auto lamda = [this, & result]
    {
        if(event == EVT_NOEVENT)
        {
            return false;
        }
        result = event;
        event = EVT_NOEVENT;
        return true;
    };
    unique_lock<mutex> lock(mtx);
    cond.wait(lock, lamda);
    return result;
}
void ProcessControl::main(size_t id)
{
    switch(id)
    {
    case 0: watch_stdout(); break;
    case 1: watch_stderr(); break;
    default:                break;
    }
}
void ProcessControl::watch_stdout(void)
{
    string str;
    while(std::getline(ps_out, str))
    {
        string head = str.substr(0, 4);
        if(head == "TO1:")
        {
            wakeup(EVT_TimeOut1);
        }
        else if(head == "TO2:")
        {
            wakeup(EVT_TimeOut2);
        }
        else if(head == "TO3:")
        {
            wakeup(EVT_TimeOut3);
        }
        else
        {
            auto pos = str.find_first_of("GAP:");
            if(0 == pos)
            {
                wakeup(EVT_Gap);
                str = str.erase(pos, (pos+4));
            }
            if("" != str)
            {
                res += str;
            }
        }
    }
}
void ProcessControl::watch_stderr(void)
{
    string str;
    while(std::getline(ps_err, str))
    {
        cout << str << endl;
    }
}

static void printParameter(string & res, size_t recid, ConstArrayMap<size_t, const char *> & pram_str)
{
    const size_t * fmt = tblRecFmt[recid];
    size_t pcnt =  tblRecSize[recid][0];
    string data = res.substr(6, (res.size() - 10));
    string ascii = "";
    for(int idx = 0; (idx + 4) <= data.size(); idx += 4)
    {
        ascii += data.substr(idx+2, 2);
        ascii += data.substr(idx  , 2);
    }
    cout << "  str1(16 bit big endian):    " << data << " : ";
    for(int idx = 0; idx < data.size(); idx += 2)
    {
        int num = stoi(data.substr(idx, 2), nullptr, 16);
        char ch = num;
        printf("%c", ch);
    }
    cout << endl;
    cout << "  str2(16 bit little endian): " << ascii << " : ";
    for(int idx = 0; idx < ascii.size(); idx += 2)
    {
        int num = stoi(ascii.substr(idx, 2), nullptr, 16);
        char ch = num;
        printf("%c", ch);
    }
    cout << endl;
    for(int idx=0; idx < pcnt; idx++)
    {
        auto paramid = fmt[idx];
        unsigned char types = getType(paramid);
        char buff[64];
        cout << "    ";
        sprintf(buff, "%2d", idx);
        string str_no(buff);
        sprintf(buff, "param id(%3ld)", paramid);
        string str_pid(buff);
        sprintf(buff, "%-8s", str_types[types]);
        string str_type(buff);
        cout << "    " << str_no << " " << str_pid << " " << str_type;
        if(types <= 2)
        {   /* uint32, int32, float */
            string reg1 = data.substr(0, 4); data = data.erase(0, 4);
            string reg2 = data.substr(0, 4); data = data.erase(0, 4);
            string reg = reg2 + reg1;
            union DWord temp;
            sscanf(reg.c_str(), "%08X", &(temp.uint32));
            printf("%08X", temp.uint32);
            char str[256];
            if(types == 2)
            {
                sprintf(str, "(%f)", temp.value);
            }
            else
            {
                sprintf(str, "(%d)", temp.uint32);
            }
            if(pram_str.has(paramid))
            {
                printf("%-15s: %s", str, pram_str[paramid]);
            }
            else
            {
                printf("%s", str);
            }
        }
        else
        {   /* uint16, int16, uint8, int8 */
            string reg = data.substr(0, 4); data = data.erase(0, 4);
            union DWord temp;
            sscanf(reg.c_str(), "%04hX", &(temp.uint16));
            char str[256];
            cout <<  reg;
            sprintf(str, "(%d)", temp.uint16);
            if(pram_str.has(paramid))
            {
                printf("%-19s: %s", str, pram_str[paramid]);
            }
            else
            {
                printf("%s", str);
            }
        }
        cout << endl;
    }
}


/**
 * read all parameter
 * uart bourdrate of 19200, parity of none, stop 2 bit
 *
 * @param name_device   device name of open serial
 */
void readAllParameter(const char * name_device)
{
    ConstArrayMap<size_t, size_t>         param_addr(tbl_no_rec_R_ModbusAddress, tbl_fmt_rec_R_ModbusAddress, __ArrayCount(tbl_no_rec_R_ModbusAddress));
    ConstArrayMap<size_t, unsigned short> fixed0(tbl0_uint16_list, tbl0_uint16, __ArrayCount(tbl0_uint16_list));
    ConstArrayMap<size_t, const char *>   pram_str(tbl_strings_keys, tbl_strings, __ArrayCount(tbl_strings_keys));
    ConstArrayMap<size_t, size_t>         grp(grp_no_G_Modbus, grp_fmt_G_Modbus, __ArrayCount(grp_no_G_Modbus));
    try
    {
        char cmd[64];
        sprintf(cmd, "smon %s -b B19200N2", name_device);
        ProcessControl mon(cmd);
        while(ProcessControl::EVT_TimeOut3 != mon.wait());
        for(auto & key: grp_no_G_Modbus)
        {
            auto recid   = grp[key];
            auto hr_addr = fixed0[param_addr[key]];
            auto hr_str  = pram_str[param_addr[key]];
            const size_t * fmt = tblRecFmt[recid];
            size_t pcnt =  tblRecSize[recid][0];
            size_t qentites = 0;
#if 0
            printf("recid(%d): HR(%d)\n", recid, hr_addr);
#endif
            printf("Read HR(%d): %s\n", hr_addr, hr_str);
            for(int idx=0; idx < pcnt; idx++)
            {
                auto paramid = fmt[idx];
                unsigned char types = getType(paramid);
                qentites ++;
                if(types < 3)
                {
                    qentites ++;
                }
            }
            string req;
            {
                char buff[64];
                sprintf(buff, "0103%04X%04lX", hr_addr, qentites);
                req += buff;
                string crc;
                calcModbusCRC(crc, req);
                req += crc;
            }
            cout << "  req: " << req << endl;
            mon.in(req);

            while(ProcessControl::EVT_Gap != mon.wait());
            string res = mon.response();
            mon.clear();
            res = res.substr(req.size(), (res.size() - req.size()));
            cout << "       ";
            for(int idx = 0; (idx + 8) < req.size(); idx ++)
            {
                cout << " ";
            }
            cout << "-> res: ";
            cout << res << endl;
            printParameter(res, recid, pram_str);
            cout << endl;
        }
        while(ProcessControl::EVT_TimeOut3 != mon.wait());
        mon.in("exit");
    }
    catch(...)
    {
        cout << "Exception" << endl;
    }
}


void Monitor(const char * name_device)
{
    ConstArrayMap<size_t, size_t>         param_addr(tbl_no_rec_R_ModbusAddress, tbl_fmt_rec_R_ModbusAddress, __ArrayCount(tbl_no_rec_R_ModbusAddress));
    ConstArrayMap<size_t, unsigned short> fixed0(tbl0_uint16_list, tbl0_uint16, __ArrayCount(tbl0_uint16_list));
    ConstArrayMap<size_t, const char *>   pram_str(tbl_strings_keys, tbl_strings, __ArrayCount(tbl_strings_keys));
    ConstArrayMap<size_t, size_t>         grp(grp_no_G_Modbus, grp_fmt_G_Modbus, __ArrayCount(grp_no_G_Modbus));
    map<string, size_t>  req_list;
    map<size_t, size_t>  addr_list;

    for(auto & key: grp_no_G_Modbus)
    {
        auto recid   = grp[key];
        auto hr_addr = fixed0[param_addr[key]];
        const size_t * fmt = tblRecFmt[recid];
        size_t pcnt =  tblRecSize[recid][0];
        size_t qentites = 0;
        addr_list[recid] = hr_addr;
        for(int idx=0; idx < pcnt; idx++)
        {
            auto paramid = fmt[idx];
            unsigned char types = getType(paramid);
            qentites ++;
            if(types < 3)
            {
                qentites ++;
            }
        }
        for(unsigned char fc = 3; fc <= 4; fc ++)
        {
            string req;
            char buff[64];
            sprintf(buff, "01%02x%04X%04lX", fc, hr_addr, qentites);
            req += buff;
            string crc;
            calcModbusCRC(crc, req);
            req += crc;
            req_list[req] = recid;
        }
    }
    try
    {
        char cmd[64];
        sprintf(cmd, "smon %s -b B19200N2", name_device);
        ProcessControl mon(cmd);
        while(ProcessControl::EVT_TimeOut3 != mon.wait());
        while(1)
        {
            while(ProcessControl::EVT_Gap != mon.wait());
            string res = mon.response();
            mon.clear();

            bool flag = true;
            for(const auto& [req, recid] : req_list)
            {
                auto rcv_req = res.substr(0, req.size());
                if(rcv_req == req)
                {
                    flag = false;
                    cout << rcv_req << endl;
                    for(int idx=0; idx < req.size(); idx ++)
                    {
                        cout << " ";
                    }
                    res = res.erase(0, req.size());
                    cout << res << endl;
                    printParameter(res, recid, pram_str);
                }
            }
            if(flag)
            {
                cout << res << endl;
            }
        }
        mon.in("exit");
    }
    catch(...)
    {
        cout << "Exception" << endl;
    }
}

/**
 * main
 *
 * @param argc      Argment Count
 * @param argv      Argument Parameter List
 * @return fixed(0)
 */
int main(int argc, char * argv[])
{
    static const char name[] = {"/dev/ttyUSB0"};
    const char * name_device = name;
    int mode = 0;
    {
        string opt0("--all");
        string opt1("--mon");
        for(int idx = 1; idx < argc; idx ++)
        {
            if(opt0 == argv[idx])
            {
                mode = 0;
            }
            else if(opt1 == argv[idx])
            {
                mode = 1;
            }
            else
            {
                name_device = argv[1];
            }
        }
    }
    switch(mode)
    {
    case 1:
        Monitor(name_device);
        break;
    case 0:
    default:
        readAllParameter(name_device);
        break;
    }
    return 0;
}
