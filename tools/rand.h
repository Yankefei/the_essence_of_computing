#ifndef _TOOLS_RAND_H_
#define _TOOLS_RAND_H_

#include <random>
#include <chrono>
#include <cstring>
#include <cassert>

namespace tools
{

template<typename T = int>
class Rand
{
    //这两种方式等价
    //typedef typename std::uniform_int_distribution<T>::param_type _param_type;
    using _param_type = typename std::uniform_int_distribution<T>::param_type;
public:
    Rand(T min = 0, T max = 0):
        seed_(std::chrono::high_resolution_clock::now().time_since_epoch().count())
    {
        assert(min <= max);

        dist_.param(_param_type(min, max));
    }

    T operator()()
    {
        return dist_(rand_generator_);
    }

    void ChangeRange(T min, T max)
    {
        dist_.param(_param_type(min, max));
    }

    T LeftBorder()  { return dist_.min();}
    T RightBorder() { return dist_.max();}

private:
    uint32_t seed_;
    std::mt19937  rand_generator_{seed_};

    std::uniform_int_distribution<T> dist_;
};

}


#endif