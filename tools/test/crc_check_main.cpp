#include "stream.h"

#include "crc_check.h"

using namespace tools;

int main()
{

    unsigned char data1[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    unsigned char data2[] = {'5', '6', '7', '8', '9'};
    unsigned short c1, c2;
    c1 = sample_crc16_check(data1, 9, 0xffff);
    c2 = sample_crc16_check(data1, 4, 0xffff);
    c2 = sample_crc16_check(data2, 5, c2);
    printf("%04x\n", c1);
    printf("%04x\n", c2);


    // crc_init();
    // c1 = crc16_check(data1, 9);
    // printf("%04x\n", c1);

    return 0;
}

