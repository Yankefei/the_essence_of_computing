#include "stream.h"

#include "map.hpp"
#include <map>

using namespace tools;

int main()
{

    stream << "sizeof std_map: "<< sizeof(std::map<int, int>) << std::endl;
    /*
    stream << "sizeof r_map: "<< sizeof(RMap<int, int>) << std::endl;

    {
        AMap<int, int> a_test_map; 
    }

    {
        RMap<int, int> r_test_map;
    }
    */
    return 0;
}