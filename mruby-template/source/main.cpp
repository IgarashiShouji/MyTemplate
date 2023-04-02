#include <boost/program_options.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <regex>

#include <mruby.h>
#include <mruby/proc.h>
#include <mruby/data.h>
#include <mruby/compile.h>
#include <mruby/string.h>
#include <mruby/hash.h>
#include <mruby/variable.h>

using namespace boost;
using namespace std;

static void print_help_message(boost::program_options::options_description & desc)
{
    cout << endl;
    cout << "mruby template test program: Software Revision 0.0.1" << endl;
    cout << endl;
    cout << " > test [Options]"<< endl;
    cout << endl;
    cout << desc << endl;
}

int main(int argc, char *argv[])
{
    using namespace boost::program_options;
    try
    {
        options_description desc("Options");
        desc.add_options()
            ("mruby-script,m", value<string>(), "execute mruby script")
            ("help,h",                          "help");
        variables_map argmap;
        store(parse_command_line(argc, argv, desc), argmap);
        notify(argmap);
        if(argmap.count("help"))
        {
            print_help_message(desc);
            return 0;
        }
        if(argmap.count("mruby-script"))
        {
            string mruby_fname = argmap["mruby-script"].as<string>();
            ifstream fin(mruby_fname);
            if(fin.is_open())
            {
                mrb_state * mrb = mrb_open();
                string code((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
                mrb_load_string(mrb, code.c_str());
                mrb_close(mrb);
                return 0;
            }
            else
            {
                string code("");
                auto split = [](std::string src, auto pat)
                {
                    std::vector<std::string> result{};
                    std::regex reg{pat};
                    std::copy( std::sregex_token_iterator{src.begin(), src.end(), reg, -1}, std::sregex_token_iterator{}, std::back_inserter(result) );
                    return result;
                };
                bool noerror(true);
                string src = argmap["mruby-script"].as<string>();
                for(auto&& fname : split(src, ","))
                {
                    ifstream fin(fname);
                    if(fin.is_open())
                    {
                        string script((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
                        code += script;
                    }
                    else
                    {
                        noerror = false;
                        printf("file open error: %s\n", fname.c_str());
                        break;
                    }
                }
                if(noerror)
                {
                    mrb_state * mrb = mrb_open();
                    mrb_load_string(mrb, code.c_str());
                    mrb_close(mrb);
                    return 0;
                }
            }
        }
        print_help_message(desc);
    }
    catch(const boost::program_options::error_with_option_name & exp)
    {
        cout << exp.what() << endl;
    }
    catch(const std::exception & exp)
    {
        printf("Open Error\n");
        std::cout << exp.what() << std::endl;
    }
    catch(...)
    {
        printf("Other Error\n");
    }
    return -1;
}
