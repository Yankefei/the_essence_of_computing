#include "stream.h"
#include <string.h>

using namespace tools;

namespace p_11_2
{

char array[] = "A bounded multi-producer multi-consumer concurrent queue written in C++11";

void change(char* ptr1, char* ptr2)
{
    char temp = *ptr1;
    *ptr1 = *ptr2;
    *ptr2 = temp;
}

void flip_word(char* array, int len)
{
    int begin_index = len / 2;

    char* end_array = array + len - 1;
    for (int i = 0; i < begin_index; i++)
    {
        change(array + i, end_array - i);
    }
}

void flip(char* array, int len)
{
    flip_word(array, len);

    // 11++C ni nettirw eueuq tnerrucnoc remusnoc-itlum recudorp-itlum dednuob A
    stream << array << std::endl;

    char* world_begin = nullptr;
    if (array[0] != ' ')
        world_begin = array;
    for (int i = 1; i <len; i++)
    {
        if (array[i] == ' ' && world_begin != nullptr)
        {
            flip_word(world_begin, array + i - world_begin);
            world_begin = nullptr;
        }

        if (world_begin == nullptr && array[i] != ' ')
        {
            world_begin = array + i;
        }
    }
    // C++11 in written queue concurrent multi-consumer multi-producer bounded A
    stream << array << std::endl;
}

}

void flip()
{
    p_11_2::flip(p_11_2::array, strlen(p_11_2::array));
}