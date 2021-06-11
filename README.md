# MyTemplate

このプロジェクトは、プログラムのテンプレート集を記載する。

## リスト

| <center>No.</center>| <center>表題</center> | <center>概要</center> |
|---:|:---|:---|
|  1 | WorkerThrad | 固定数のワーカースレッド生成・管理クラス(stl:thread版)のテンプレート |
|  2 | TextFilter  | テキストファイル・フィルターのテンプレート |

---

## WoekerThread

固定数のワーカースレッドを生成し、時間短縮を行う際によく作成するスレッド管理オブジェクトのテンプレート。

### サンプル

~~~cpp
#include <iostream>
class TestThread : public WorkerThread
{
public:
    TestThread(void);
    virtual ~TestThread(void);
    virtual void main(size_t id, unsigned int event);
    void operator() (void);
};

TestThread::TestThread(void)
//  : WorkerThread(8)
{
}

TestThread::~TestThread(void)
{
}

void TestThread ::main(size_t id, unsigned int event)
{
    printf("  ID(%d): %08x\n", static_cast<unsigned int>(id), event);
    if(0xf & event)
    {
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

void TestThread::operator() (void)
{
    for(size_t cnt=0; cnt<10; cnt ++)
    {
        static const size_t list[] = { 0x01, 0x04, 0x18, 0x10, 0x20, 0x40, 0x80 };
        for(auto event: list)
        {
            size_t id = getWaitTask();
            setEvent(id, event);
        }
        waitEmptyEvent();
    }
}

int main(int argc, char * argv[])
{
    try
    {
        cout << "create" << endl;
        TestThread tes;
        cout << "start" << endl;
        tes();
        cout << "end" << endl;
    }
    catch(exception & exp)
    {
        cout << exp.what() << endl;
    }
    catch(...)
    {
        cout << "Ohter exceptions" << endl;
    }
    fflush(stdout);
    return 0;
}
~~~

### 依存

* stl::thrad

### 備考

なし

---

## TextFilter

テキストファイル用のフィルタープログラムサンプル

### サンプル

~~~cpp
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
        TextFilter tes;
        tes();
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
~~~

### 依存

* std::fstream
* std::iostream
* std::vector
* std::cstring
* std::cstdio

### 備考

なし
