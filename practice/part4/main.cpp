#include <iostream>

#include "stream.h"

using namespace tools;

extern long long n_char_merge_1(int len);

extern long long n_char_merge_2(int len);

extern uint64_t work_n_n_1(uint32_t m, uint32_t n);
extern uint64_t work_n_n_2(uint32_t m, uint32_t n);

int main()
{
    std::cout <<"3: "<< n_char_merge_1(3) << std::endl;
    std::cout <<"4: "<< n_char_merge_1(4) << std::endl;
    std::cout <<"5: "<< n_char_merge_1(5) << std::endl;

    std::cout <<"3: "<< n_char_merge_2(3) << std::endl;
    std::cout <<"4: "<< n_char_merge_2(4) << std::endl;
    std::cout <<"5: "<< n_char_merge_2(5) << std::endl;

    std::cout <<"1, 1 "<< work_n_n_1(1, 1) << std::endl;
    std::cout <<"2, 2 "<< work_n_n_1(2, 2) << std::endl;
    std::cout <<"3, 3 "<< work_n_n_1(3, 3) << std::endl;
    std::cout <<"4, 4 "<< work_n_n_1(4, 4) << std::endl;
    std::cout <<"5, 5 "<< work_n_n_1(5, 5) << std::endl;

    std::cout <<"1, 1 "<< work_n_n_2(0, 0) << std::endl;
    std::cout <<"1, 1 "<< work_n_n_2(1, 1) << std::endl;
    std::cout <<"2, 2 "<< work_n_n_2(2, 2) << std::endl;
    std::cout <<"3, 3 "<< work_n_n_2(3, 3) << std::endl;
    std::cout <<"4, 4 "<< work_n_n_2(4, 4) << std::endl;

    return 0;
}