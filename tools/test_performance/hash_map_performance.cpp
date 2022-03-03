#include <iostream>
#include "stream.h"

#include <unordered_map>
#include "hash_map.hpp"

#include "vector.hpp"
#include "timer_guard.h"
#include "rand.h"

using namespace tools;

int val_size;

void test_for_hash_map()
{
    Vector<int> array;
    Rand<int> rand(1, val_size * 10);
    for(int i = 0; i < val_size; i++)
    {
        array.push_back(rand());
    }

    HashMap<int, int> test_map;
    {
        TimerGuard timer("hash_map insert:");
        for (auto & e : array)
        {
            test_map.insert(e, e);
        }
    }
    stream << "hash_map size: " << test_map.size() << std::endl;
    {
        TimerGuard timer("hash_map find:");
        for (auto& e : array)
        {
            if (test_map.find(e) == test_map.end())
                stream << "hash_map find error: " << e << std::endl;
        }
    }
    {
        TimerGuard timer("hash_map loop erase:");
        for (auto it = test_map.begin(); it != test_map.end();)
            it = test_map.erase(it);
    }
    if (!test_map.empty())
        stream << "hash_map delete error" << std::endl;
}

void test_for_unordered_map()
{
    Vector<int> array;
    Rand<int> rand(1, val_size * 10);
    for(int i = 0; i < val_size; i++)
    {
        array.push_back(rand());
    }

    std::unordered_map<int, int> test_map;
    {
        TimerGuard timer("unordered_map insert:");
        for (auto & e : array)
        {
            test_map.insert(std::pair<int, int>(e, e));
        }
    }
    stream << "unordered_map size: " << test_map.size() << std::endl;
    {
        TimerGuard timer("unordered_map find:");
        for (auto& e : array)
        {
            if (test_map.find(e) == test_map.end())
                stream << "unordered_map find error: " << e << std::endl;
        }
    }
    {
        TimerGuard timer("unordered_map loop erase:");
        for (auto it = test_map.begin(); it != test_map.end();)
            it = test_map.erase(it);
    }
    if (!test_map.empty())
        stream << "unordered_map delete error" << std::endl;
}

int main()
{
    /* 环境参数：
       Release版，Centos7, gcc 7.5
       Intel(R) Core(TM) i5-6200U CPU 2核（共4核，分配给虚拟机2个核）
       结论：
       总体来说，HashMap在插入，删除时不稳定，因为插入，删除的过程中使用了固定长度的数组，数组扩展的时候
       需要拷贝数据和重新Hash, 导致在出现较多数据的时候，导致时延较大。相比之下unordered_map的链地址法
       稳定很多。
       但是在数据变化不大，且查询密集的情况下，HashMap[开放地址法(平方探测)]的优势才能体现出来，查询时延
       优于unordered_map.
    */

/*
size: 10000
Tag: hash_map insert:    , diff_nas: 136025971, As micro: 136025, As milli: 136
hash_map size: 9535
Tag: hash_map find:      , diff_nas: 298209, As micro: 298
Tag: hash_map loop erase:, diff_nas: 34763844, As micro: 34763, As milli: 34

size: 100000
time1:
Tag: hash_map insert:    , diff_nas: 116845237032, As micro: 116845237, As milli: 116845, As sec: 116
hash_map size: 95138
Tag: hash_map find:      , diff_nas: 9354682, As micro: 9354, As milli: 9
Tag: hash_map loop erase:, diff_nas: 25141141152, As micro: 25141141, As milli: 25141, As sec: 25
time2:
Tag: hash_map insert:    , diff_nas: 135990736524, As micro: 135990736, As milli: 135990, As sec: 135
hash_map size: 95075
Tag: hash_map find:      , diff_nas: 6754822, As micro: 6754, As milli: 6
Tag: hash_map loop erase:, diff_nas: 35009904453, As micro: 35009904, As milli: 35009, As sec: 35

*/
    // val_size = 100000;
    // test_for_hash_map();

/*
size: 10000
Tag: unordered_map insert, diff_nas: 3232632, As micro: 3232, As milli: 3
unordered_map size: 9518
Tag: unordered_map find: , diff_nas: 364622, As micro: 364
Tag: unordered_map loop e, diff_nas: 466441, As micro: 466

size: 100000
time1:
Tag: unordered_map insert, diff_nas: 42250420, As micro: 42250, As milli: 42
unordered_map size: 95142
Tag: unordered_map find: , diff_nas: 11944360, As micro: 11944, As milli: 11
Tag: unordered_map loop e, diff_nas: 12070304, As micro: 12070, As milli: 12
time2:
Tag: unordered_map insert, diff_nas: 43414602, As micro: 43414, As milli: 43
unordered_map size: 95299
Tag: unordered_map find: , diff_nas: 12818488, As micro: 12818, As milli: 12
Tag: unordered_map loop e, diff_nas: 12330642, As micro: 12330, As milli: 12


size: 10000000
Tag: unordered_map insert, diff_nas: 10285835813, As micro: 10285835, As milli: 10285, As sec: 10
unordered_map size: 9950139
Tag: unordered_map find: , diff_nas: 2465812420, As micro: 2465812, As milli: 2465, As sec: 2
Tag: unordered_map loop e, diff_nas: 3265082148, As micro: 3265082, As milli: 3265, As sec: 3

*/
    val_size = 100000;
    test_for_unordered_map();

    return 0;
}