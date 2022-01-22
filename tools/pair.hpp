#ifndef _TOOLS_PAIR_H_
#define _TOOLS_PAIR_H_

#include <cstdint>

namespace tools
{

template<typename T1, typename T2>
struct Pair
{
    Pair(const T1& _t1, const T2& _t2) : first(_t1), second(_t2) {}

    T1 first;
    T2 second;
};

template<typename T1, typename T2>
bool operator==(const Pair<T1, T2>& lhs, const Pair<T1, T2>& rhs)
{
    return lhs.first == rhs.first;
}

template<typename T1, typename T2>
bool operator!=(const Pair<T1, T2>& lhs, const Pair<T1, T2>& rhs)
{
    return !(lhs == rhs);
}

template<typename T1, typename T2>
bool operator>(const Pair<T1, T2>& lhs, const Pair<T1, T2>& rhs)
{
    return lhs.first > rhs.first;
}



}


#endif