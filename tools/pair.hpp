#ifndef _TOOLS_PAIR_H_
#define _TOOLS_PAIR_H_

#include <cstdint>
#include "algorithm.hpp"

namespace tools
{

template<typename T1, typename T2>
struct Pair
{
    Pair() = default;
    Pair(const T1& _t1, const T2& _t2) : first(_t1), second(_t2) {}
    Pair(T1&& _t1, T2&& _t2) : first(std::move(_t1)), second(std::move(_t2)) {}
    Pair(const Pair& val) : first(val.first), second(val.second) {}

    T1 first;
    T2 second;
};

template<typename T1, typename T2>
Pair<T1, T2>
    MakePair(const T1& t1, const T2& t2)
{
    Pair<T1, T2> pair(t1, t2);
    return std::move(pair);
}

template<typename T1, typename T2>
Pair<T1, T2>
    MakePair(T1&& t1, T2&& t2)
{
    Pair<T1, T2> pair(std::forward(t1), std::forward(t2));
    return std::move(pair);
}

template<typename T1, typename T2>
bool operator==(const Pair<T1, T2>& lhs, const Pair<T1, T2>& rhs)
{
    return alg::eq(lhs.first, rhs.first);
}

template<typename T1, typename T2>
bool operator!=(const Pair<T1, T2>& lhs, const Pair<T1, T2>& rhs)
{
    return !lhs == rhs;
}

template<typename T1, typename T2>
bool operator<(const Pair<T1, T2>& lhs, const Pair<T1, T2>& rhs)
{
    return alg::le(lhs.first, rhs.first);
}

template<typename T1, typename T2>
bool operator>(const Pair<T1, T2>& lhs, const Pair<T1, T2>& rhs)
{
    return alg::gt(lhs.first, rhs.first);
}



}


#endif