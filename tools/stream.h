#ifndef _TOOLS_STREAM_H_
#define _TOOLS_STREAM_H_

#include <ostream>
#include <vector>
#include <initializer_list>

namespace tools
{

class Stream
{
public:
    Stream(std::initializer_list<std::ostream*> os_il)
    {
        for(auto p = os_il.begin(); p != os_il.end(); p ++)
            os_.emplace_back(*p);
    }

    ~Stream()
    {}

    Stream& AddStream(std::ostream* os)
    {
        os_.emplace_back(os);
        return *this;
    }

    template<typename T>
    Stream& operator<<(const T& data)
    {
        for (auto& os : os_)
            *os<< data;
        return *this;
    }

    Stream& operator <<(bool val)
    {
        for (auto&os : os_)
        {
            if (is_set_boolalpha_)
            {
                if (val)
                    *os << "true";
                else
                    *os << "false";
            }
            else
            {
                *os << val;
            }
        }
        return *this;
    }

    // template<typename T>
    // Stream& operator<<(T&& data)
    // {
    //     for (auto& os : os_)
    //         *os<< std::forward<T>(data);
    //     return *this;
    // }

    Stream& operator<<(const std::string& str)
    {
        for (auto& os : os_)
            *os<< str;
        return *this;
    }

    // << std::endl
    using ostream_type = std::ostream::__ostream_type;
    Stream& operator<<(ostream_type&(*func)(ostream_type&))
    {
        for (auto& os : os_)
            func(*os);
        return *this;
    }

    // << Boolalpha or << NoBoolalpha
    Stream& operator<<(void(*func)(Stream&))
    {
        func(*this);
        return *this;
    }

    static void set_boolalpha(Stream& os, bool val) { os.is_set_boolalpha_ = val; }

private:
    std::vector<std::ostream*> os_;

    bool is_set_boolalpha_{false};
};


extern void Boolalpha(Stream& os);
extern void NoBoolalpha(Stream& os);

extern Stream stream;

}

#endif