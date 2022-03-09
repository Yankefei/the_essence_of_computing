#include <iostream>
#include <string.h>
#include "stream.h"

extern void eight_queues1();

extern void eight_queues2();

extern void eight_queues3();

extern void eight_queens4();

extern uint32_t calculator(const char* buf, uint32_t size);

extern uint32_t calculator2(const char* buf, uint32_t size);

int main()
{
    // eight_queues1();

    // eight_queues2();

    // eight_queues3();

    // eight_queens4();

    char buffer[512] = {0};

    fgets(buffer, sizeof(buffer), stdin);

    // 5+3*2=
    tools::stream << "get buffer: " << buffer << std::endl;

    int len = strlen(buffer) - 1;
    if (buffer[len -1]  != '=')
    {
        tools::stream << "invalid buffer, no '=' operator" << std::endl;
        return 0;
    }
    //tools::stream << calculator(buffer, strlen(buffer) - 1) << std::endl;
    tools::stream << calculator2(buffer, len) << std::endl;

    return 0;
}