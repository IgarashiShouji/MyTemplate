/**
 * @file RegexFilter.cpp
 * @brief 
 *    標準入力のテキストデータを正規表現で置換して標準出力するテンプレート
 * @copyright Copyright (c) 2021 igarashi
 */
#include <iostream>
#include <string>
#include <regex>
#include <cstdlib>

using namespace std;

/**
 * @brief mein
 * getTest_01239 test string
 * @param argc      プログラムの起動オプション数
 * @param argv      プログラムの起動オプション文字列リスト
 * @return int      0 固定
 */
int main(int argc, char * argv[])
{
    regex reg(R"((\bget[0-9a-zA-Z_]+\b))");
    string str;
    size_t line = 0;
    while(getline(cin, str))
    {
        if(regex_search(str, reg))
        {
            cout << "--- << orgiginal >> ---" <<  endl;
            cout.width(4); cout << line << ": " << str << endl;
            cout << "--- <<  replace  >> ---" <<  endl;
            str = regex_replace(str, reg, "_$1");
            cout.width(4); cout << line << ": " << str << endl;
            cout << "-----------------------" << endl;
        }
        else
        {
            cout.width(4); cout << line << ": " << str << endl;
        }
        line ++;
    }
    return 0;
}