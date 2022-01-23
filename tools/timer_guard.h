#ifndef _TOOLS_TIMER_GUARD_H_
#define _TOOLS_TIMER_GUARD_H_

#include <chrono>
#include <ctime>
#include <string>

#include "stream.h"

namespace tools
{

class TimerGuard
{
public:
    TimerGuard(const std::string& remark = "", bool is_start_read_now = true)
    {
        ResetRemark(remark);
        if (is_start_read_now)
            StartGetTime();
    }

    ~TimerGuard()
    {
        StopGetTime();
    }

    bool StartGetTime()
    {
        if (is_start_) return false;

        is_start_ = true;
        ::clock_gettime(CLOCK_REALTIME, &begin_time_);
    }

    bool StopGetTime()
    {
        if (!is_start_) return false;

        ::clock_gettime(CLOCK_REALTIME, &end_time_);
        uint64_t diff_nas = to_nanoseconds_int_max(end_time_) -
                            to_nanoseconds_int_max(begin_time_);
        uint64_t diff_micro = diff_nas / 1000l;
        uint64_t diff_milli = diff_micro / 1000l;
        uint64_t diff_sec = diff_milli / 1000l;
        stream << "Tag: "<< remark_ <<", diff_nas: "<< diff_nas;
        if (diff_micro)
            stream << ", As micro: " << diff_micro;
        if (diff_milli)
            stream << ", As milli: " << diff_milli;
        if (diff_sec)
            stream << ", As sec: " << diff_sec;
        
        stream << std::endl;

        is_start_ = false;
    }

    void ResetTime()
    {
        is_start_ = false;
        begin_time_ = end_time_ = {0, 0};
    }

    void ResetRemark(const std::string& str)
    {
        remark_ = str;
        remark_.resize(20, ' ');
    }

private:
    uint64_t to_nanoseconds_int_max(timespec const& ts)
    {
      return static_cast<uint64_t>(ts.tv_sec) * 1000000000l + ts.tv_nsec;
    }

private:
    std::string remark_;
    bool is_start_{false};

    timespec begin_time_{0};
    timespec end_time_{0};
};

}

#endif