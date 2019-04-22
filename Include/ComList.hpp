#ifndef __COM_LIST_HPP__
#define __COM_LIST_HPP__

#include <string>
#include <vector>

class ComList
{
private:
    std::vector<std::string> list;
public:
    ComList(void);
    virtual ~ComList(void);
    std::vector<std::string> & ref();
};

#endif
