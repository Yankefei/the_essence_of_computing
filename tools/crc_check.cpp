#include "crc_check.h"



namespace tools
{
/*
                 CCITT            CRC16               CRC32

校验和位宽W         16               16                  32

生成多项式     x16+x12+x5+1     x16+x15+x2+1          x32+x26+x23+x22+x16+
                                                     x12+x11+x10+x8+x7+x5+
                                                         x4+x2+x1+1

除数（多项式）   0x1021            0x8005              0x04C11DB7

余数初始值       0xFFFF            0x0000             0xFFFFFFFF

结果异或值       0x0000            0x0000               0xFFFFFFFF
*/
#define POLY   0x1021
/*
最基本的CRC除法有个很明显的缺陷，就是数据流的开头添加一些0并不影响最后校验字的结果。
这个问题很让人恼火啊，因此真正应用的CRC 算法基本都在原始的CRC算法的基础上做了些小的改动。
所谓的改动，也就是增加了两个概念，第一个是“余数初始值”，第二个是“结果异或值”。
所谓的“余数初始值”就是在计算CRC值的开始，给CRC寄存器一个初始值。“结果异或值”是在其余计算完成后将
CRC寄存器的值在与这个值进行一下异或操作作为最后的校验值。
*/

/*
    todo: 太难理解了，先放下吧
*/

/*
    MSB 二进制最左侧， LSB 二进制最右侧
（1）      设置CRC寄存器，并给其赋值为“余数初始值”。
（2）      将数据的第一个8-bit字符与CRC寄存器进行异或，并把结果存入CRC寄存器。
（3）      CRC寄存器向右移一位，LSB补零，移出并检查MSB。
（4）      如果MSB为0，重复第三步；若MSB为1，CRC寄存器与0x31相异或。
（5）      重复第3与第4步直到8次移位全部完成。此时一个8-bit数据处理完毕。
（6）      重复第2至第5步直到所有数据全部处理完成。
（7）      最终CRC寄存器的内容与“结果异或值”进行或非操作后即为CRC值。
*/
uint16_t sample_crc16_check(uint8_t* addr, int num, uint16_t crc)
{
    int i;
    for (; num > 0; num --)
    {
        // uint16_t _addr = *addr ++ << 8;
        // stream << _addr << std::endl;  // 与数据的首8bit数据左移8位后进行异或运算
        crc = crc ^ (*addr ++ << 8);
        for (i = 0; i < 8; i++)
        {
            if (crc & 0x8000)
                crc = (crc << 1) ^ POLY;
            else
                crc <<=1;
        }
    }
    return crc; // crc ^ 0x0000;
}

// 计算本字节后的CRC码，等于上一字节余式CRC码的低8位左移8位。加上上一字节CRC右移8位和本字节之和
// 所得出的CRC码。

#define POLYNOMIAL          0x1021
#define INITIAL_REMAINDER   0xFFFF
#define FINAL_XOR_VALUE     0x0000

#define WIDTH   16
#define TOPBIT  ( 1 << (WIDTH - 1))

static uint16_t crc_table[256];

void crc_init()
{
    uint16_t remainder;
    uint16_t dividend;
    int bit;
    /* Perform binary long division, a bit at a time. */
    for(dividend = 0; dividend < 256; dividend++)
    {
        /* Initialize the remainder.  */
        remainder = dividend << (WIDTH - 8);
        /* Shift and XOR with the polynomial.   */
        for(bit = 0; bit < 8; bit++)
        {
            /* Try to divide the current data bit.  */
            if(remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL;
            }
            else
            {
                remainder = remainder << 1;
            }
        }
        /* Save the result in the table. */
        crc_table[dividend] = remainder;
    }
}

uint16_t crc16_check(uint8_t* addr, int num)
{
    unsigned int offset;
    unsigned char byte;
    uint16_t remainder = INITIAL_REMAINDER;
    /* Divide the addr by the polynomial, a byte at a time. */
    for( offset = 0; offset < num; offset++)
    {
        byte = (remainder >> (WIDTH - 8)) ^ addr[offset];
        remainder = crc_table[byte] ^ (remainder << 8);
    }
    /* The final remainder is the CRC result. */
    return (remainder ^ FINAL_XOR_VALUE);
}


}