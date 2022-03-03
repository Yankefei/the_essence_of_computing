#include <iostream>
#include "stream.h"

#include <map>
#include "map.hpp"

#include "vector.hpp"
#include "timer_guard.h"
#include "rand.h"

using namespace tools;

int val_size;

void test_for_RMap()
{
    Vector<int> array;
    Rand<int> rand(1, val_size * 10);
    for(int i = 0; i < val_size; i++)
    {
        array.push_back(rand());
    }

    RMap<int, int> test_map;
    {
        TimerGuard timer("RMap insert:");
        for (auto & e : array)
        {
            test_map.insert(Pair<int, int>(e, e));
        }
    }
    stream << "RMap size: " << test_map.size() << std::endl;
    {
        TimerGuard timer("RMap find:");
        for (auto& e : array)
        {
            if (test_map.find(e) == test_map.end())
                stream << "RMap find error: " << e << std::endl;
        }
    }
    {
        TimerGuard timer("RMap loop erase:");
        for (auto it = test_map.begin(); it != test_map.end();)
            it = test_map.erase(it);
    }
    if (!test_map.empty())
        stream << "RMap delete error" << std::endl;
}

void test_for_AMap()
{
    Vector<int> array;
    Rand<int> rand(1, val_size * 10);
    for(int i = 0; i < val_size; i++)
    {
        array.push_back(rand());
    }

    AMap<int, int> test_map;
    {
        TimerGuard timer("AMap insert:");
        for (auto & e : array)
        {
            test_map.insert(Pair<int, int>(e, e));
        }
    }
    stream << "AMap size: " << test_map.size() << std::endl;
    {
        TimerGuard timer("AMap find:");
        for (auto& e : array)
        {
            if (test_map.find(e) == test_map.end())
                stream << "AMap find error: " << e << std::endl;
        }
    }
    {
        TimerGuard timer("AMap loop erase:");
        for (auto it = test_map.begin(); it != test_map.end();)
            it = test_map.erase(it);
    }
    if (!test_map.empty())
        stream << "AMap delete error" << std::endl;
}

void test_for_map()
{
    Vector<int> array;
    Rand<int> rand(1, val_size * 10);
    for(int i = 0; i < val_size; i++)
    {
        array.push_back(rand());
    }

    std::map<int, int> test_map;
    {
        TimerGuard timer("map insert:");
        for (auto & e : array)
        {
            test_map.insert(std::pair<int, int>(e, e));
        }
    }
    stream << "map size: " << test_map.size() << std::endl;
    {
        TimerGuard timer("map find:");
        for (auto& e : array)
        {
            if (test_map.find(e) == test_map.end())
                stream << "map find error: " << e << std::endl;
        }
    }
    {
        TimerGuard timer("map loop erase:");
        for (auto it = test_map.begin(); it != test_map.end();)
            it = test_map.erase(it);
    }
    if (!test_map.empty())
        stream << "map delete error" << std::endl;
}


int main()
{
    /* 环境参数：
       Release版，Centos7, gcc 7.5
       Intel(R) Core(TM) i5-6200U CPU 2核（共4核，分配给虚拟机2个核）
       结论：
       总体来说，基于AVL的Map和红黑树的Map相比于std::map来说，插入，查找，删除的时延基本属于同
       一水平。
    */

/*
size: 1000000
Tag: RMap insert:        , diff_nas: 1292476166, As micro: 1292476, As milli: 1292, As sec: 1
RMap size: 951582
Tag: RMap find:          , diff_nas: 1097833232, As micro: 1097833, As milli: 1097, As sec: 1
Tag: RMap loop erase:    , diff_nas: 328815960, As micro: 328815, As milli: 328

size: 10000000
time1:
Tag: RMap insert:        , diff_nas: 30372342407, As micro: 30372342, As milli: 30372, As sec: 30
RMap size: 9516264
Tag: RMap find:          , diff_nas: 26355859385, As micro: 26355859, As milli: 26355, As sec: 26
Tag: RMap loop erase:    , diff_nas: 5308953013, As micro: 5308953, As milli: 5308, As sec: 5
time2:
Tag: RMap insert:        , diff_nas: 29794845641, As micro: 29794845, As milli: 29794, As sec: 29
RMap size: 9517043
Tag: RMap find:          , diff_nas: 25467809575, As micro: 25467809, As milli: 25467, As sec: 25
Tag: RMap loop erase:    , diff_nas: 4479392162, As micro: 4479392, As milli: 4479, As sec: 4

*/
    val_size = 10000000;
    test_for_RMap();

/*
size: 1000000
Tag: AMap insert:        , diff_nas: 1733632618, As micro: 1733632, As milli: 1733, As sec: 1
AMap size: 951592
Tag: AMap find:          , diff_nas: 1553532074, As micro: 1553532, As milli: 1553, As sec: 1
Tag: AMap loop erase:    , diff_nas: 233033960, As micro: 233033, As milli: 233

size: 10000000
Tag: AMap insert:        , diff_nas: 27016347116, As micro: 27016347, As milli: 27016, As sec: 27
AMap size: 9516268
Tag: AMap find:          , diff_nas: 26483084088, As micro: 26483084, As milli: 26483, As sec: 26
Tag: AMap loop erase:    , diff_nas: 3037935355, As micro: 3037935, As milli: 3037, As sec: 3

*/
    // val_size = 10000000;
    // test_for_AMap();
/*
size: 1000000
Tag: map insert:         , diff_nas: 1578397310, As micro: 1578397, As milli: 1578, As sec: 1
map size: 951729
Tag: map find:           , diff_nas: 1482694097, As micro: 1482694, As milli: 1482, As sec: 1
Tag: map loop erase:     , diff_nas: 174630991, As micro: 174630, As milli: 174

size: 10000000
Tag: map insert:         , diff_nas: 31174066633, As micro: 31174066, As milli: 31174, As sec: 31
map size: 9514979
Tag: map find:           , diff_nas: 29577379637, As micro: 29577379, As milli: 29577, As sec: 29
Tag: map loop erase:     , diff_nas: 2214521380, As micro: 2214521, As milli: 2214, As sec: 2

*/
    // val_size = 10000000;
    // test_for_map();
    return 0;
}