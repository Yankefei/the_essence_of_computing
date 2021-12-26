#include "stream.h"

#include "vector.hpp"

using namespace tools;

template<typename T>
void Print(const tools::Vector<T>& a)
{
    stream << "size: " <<a.size() << ", cap: "<< a.capacity() << std::endl;
    for (auto e : a)
    {
        stream << e << " ";
    }
    stream << std::endl;
}


int main()
{
#if 0
    {
        tools::Vector<int> a;
        a.push_back(1);
        a.pop_back();
        a.emplace_back(123);

        a.resize(10);
        for (int i = 0; i < 9; i ++)
        {
            a.push_back(i + 1);
        }

        Print(a);

        a.erase(a.begin());
        Print(a);
        a.insert(a.begin(), 100);
        Print(a);

        a.insert(a.begin(), 100);
        Print(a);
        a.insert(a.begin(), 100);
        Print(a);

        auto a_iter = a.begin() + 5;
        stream << "a iter: "<< *a_iter << std::endl;
        a_iter = a.erase(a_iter, a_iter + 3);
        Print(a);

        stream << "a iter: "<< *a_iter << std::endl;

        stream << "------------" << std::endl;
        tools::Vector<int> b = a;

        auto iter_b = b.begin() + 5;
        stream << "b iter: "<< *iter_b << std::endl;
        Print(b);
        stream << "-----before insert-------" << std::endl;
        a_iter = a.insert(a_iter, iter_b, iter_b + 3);
        stream << "a iter: "<< *a_iter << std::endl;
        Print(a);
        
        stream << "------------" << std::endl;

        b = a;

        Print(b);
    }

#endif

    {
        tools::Vector<int> a{1,2,3,4,5,6,7};
        tools::Vector<int> b{10, 11, 12};

        b = std::move(a);

        Print(a);
        Print(b);

        tools::Vector<tools::Vector<int>> v_a;
        v_a.emplace_back(b);

        stream << v_a.size() << " " << v_a.capacity() << std::endl;
    }

    return 0;
}