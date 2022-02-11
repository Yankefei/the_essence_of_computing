#ifndef _TOOLS_HASH_FUNC_H_
#define _TOOLS_HASH_FUNC_H_

#include <bits/functional_hash.h>

namespace tools
{

// 直接继承std的hash定义类作为扩展
template<typename T>
struct Hash : std::hash<T>
{
};


}

#endif