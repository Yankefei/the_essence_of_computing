#ifndef _TOOLS_CRC_CHECK_H_
#define _TOOLS_CRC_CHECK_H_

#include <cstdint>
#include "stream.h"

namespace tools
{
// 参考自：https://blog.csdn.net/liyuanbhu/article/details/7882789
//        https://www.cnblogs.com/esestt/archive/2007/08/09/848856.html

extern uint16_t sample_crc16_check(uint8_t* addr, int num, uint16_t crc);


// 使用查表法
extern void crc_init();

extern uint16_t crc16_check(uint8_t* addr, int num);

}

#endif