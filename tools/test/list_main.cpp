#include <iostream>
#include "stream.h"

#include "string.hpp"
#include "list.hpp"
#include <list>

using namespace tools;

std::ostream& operator<<(std::ostream& ostr, const std::list<int>& list)
{
    for (auto &i : list) {
        ostr << " " << i;
    }
    return ostr;
}

int main()
{
    {
        std::list<int> tmp_list;

        stream << "sizeof std::list " << sizeof(std::list<char>) << std::endl;

        std::list<int> list1 = { 5,9,0,1,3 };
        std::list<int> list2 = { 8,7,2,6,4 };
    
        //list1.sort();
        list2.sort();
        std::cout << "list1:  " << list1 << "\n";
        std::cout << "list2:  " << list2 << "\n";
        list1.merge(list2);
        std::cout << "merged: " << list1 << "\n";

    }

    {
        List<int> my_test;
        int a = 3;
        a = a;
        stream << a << std::endl;

        my_test.push_back(1);
        my_test.push_front(2);
        for (auto & e : my_test)
            stream << e << " ";
        stream << "my_test size: "<< my_test.size() << std::endl;

        List<int> my_test_copy = my_test;
        for (auto & e : my_test_copy)
            stream << e << " ";
        stream << "my_test_copy size: "<< my_test_copy.size() << std::endl;

        List<int> my_test_move = std::move(my_test_copy);
        stream << "after move" << std::endl;
        for (auto & e : my_test_copy)
            stream << e << " ";
        stream << "my_test_copy size: "<< my_test_copy.size() << std::endl;

        for (auto & e : my_test_move)
            stream << e << " ";
        stream << "my_test_move size: "<< my_test_move.size() << std::endl;

        my_test.clear();
        my_test = std::move(my_test_move);
        stream << "after move" << std::endl;
        for (auto & e : my_test_move)
            stream << e << " ";
        stream << "my_test_move size: "<< my_test_move.size() << std::endl;

        for (auto & e : my_test)
            stream << e << " ";
        stream << "my_test size: "<< my_test.size() << std::endl;
    }

    {
        List<int> my_test;
        for (int i = 0; i < 20; i ++)
            my_test.emplace_back(i);
        stream << my_test << std::endl;
        my_test.remove(13);
        my_test.pop_back();
        my_test.pop_front();
        stream << my_test <<", size: "<< my_test.size()<< std::endl;

        List<int> my_test_copy = my_test;
        auto copy_iter = my_test_copy.end();
        copy_iter--;

        stream <<" my_test size: "<< my_test.size()<<  std::endl;
        for (auto it = my_test.begin(); it != my_test.end(); it ++)
            stream << *it << " ";
        stream << std::endl;

        stream <<" my_test_copy size: "<< my_test_copy.size()<< std::endl;
        for (auto it = my_test_copy.begin(); it != my_test_copy.end(); it ++)
            stream << *it << " ";
        stream << std::endl;

        my_test.splice(my_test.begin(), my_test_copy, copy_iter);
        
        stream <<" my_test size: "<< my_test.size()<< std::endl;
        for (auto it = my_test.begin(); it != my_test.end(); it ++)
            stream << *it << " ";
        stream << std::endl;

        stream <<" my_test_copy size: "<< my_test_copy.size()<< std::endl;
        for (auto it = my_test_copy.begin(); it != my_test_copy.end(); it ++)
            stream << *it << " ";
        stream << std::endl;

        my_test.reverse();
        stream <<" my_test size: "<< my_test.size()<< std::endl;
        for (auto it = my_test.begin(); it != my_test.end(); it ++)
            stream << *it << " ";
        stream << std::endl;

        my_test.clear();
        my_test.push_back(1);
        my_test.reverse();
        stream <<" my_test size: "<< my_test.size()<< std::endl;
        for (auto it = my_test.begin(); it != my_test.end(); it ++)
            stream << *it << " ";
        stream << std::endl;
    }
    {
        List<int> my_test1 = {1, 3, 5, 7, 9};
        List<int> my_test2 = {2, 4, 6, 8, 10};

        my_test1.merge(my_test2);
        stream <<" my_test1 size: "<< my_test1.size()<< std::endl;
        for (auto it = my_test1.begin(); it != my_test1.end(); it ++)
            stream << *it << " ";
        stream << std::endl;
    }
    {
        List<int> my_test1 = {0, 1, 3, 5, 7, 9, 11, 13};
        List<int> my_test2 = {2, 4, 6, 8, 10};

        my_test1.merge(my_test2);
        stream <<" my_test1 size: "<< my_test1.size()<< std::endl;
        for (auto it = my_test1.begin(); it != my_test1.end(); it ++)
            stream << *it << " ";
        stream << std::endl;
    }
    {
        List<int> my_test1 = {0, 1, 3, 5, 7, 9};
        List<int> my_test2 = {2, 4, 6, 8, 10, 12, 14};

        my_test1.merge(my_test2);
        stream <<" my_test1 size: "<< my_test1.size()<< std::endl;
        for (auto it = my_test1.begin(); it != my_test1.end(); it ++)
            stream << *it << " ";
        stream << std::endl;
    }
    {
        List<int> my_test = {
        222, 16, 884, 104, 740, 789, 498, 523, 525, 950,
        582, 198, 35, 571, 940, 965, 116, 922, 864, 407};
        stream <<" my_test size: "<< my_test.size()<< std::endl;
        my_test.sort();
        stream <<" my_test size: "<< my_test.size()<< std::endl;
        for (auto it = my_test.begin(); it != my_test.end(); it ++)
            stream << *it << " ";
        stream << std::endl;

        List<int> my_test2 = {2};
        my_test2.sort();
        for (auto it = my_test2.begin(); it != my_test2.end(); it ++)
            stream << *it << " ";
        stream << std::endl;
    }

    {
        List<String> my_test = {"123", "askjf", "test", "9j3", "45s", "zek"};
        my_test.sort();
        for (auto it = my_test.begin(); it != my_test.end(); it ++)
            stream << *it << " ";
        stream <<my_test.size() <<std::endl;

        List<String> my_test2 = std::move(my_test);
        for (auto it = my_test.begin(); it != my_test.end(); it ++)
            stream << *it << " ";
        stream <<my_test.size() <<std::endl;
        for (auto it = my_test2.begin(); it != my_test2.end(); it ++)
            stream << *it << " ";
        stream <<my_test2.size() <<std::endl;
    }

    return 0;
}