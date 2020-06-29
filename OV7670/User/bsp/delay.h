#ifndef _DELAY_H
#define _DELAY_H

#include "stm32f10x.h"

//微秒软件延时函数
__STATIC_INLINE void sw_delay_us(uint32_t number_of_us);
//毫秒软件延时函数
__STATIC_INLINE void sw_delay_ms(uint32_t number_of_ms);


__STATIC_INLINE void sw_delay_us(uint32_t number_of_us)
{
  //输入的延时时间为0的话，程序直接返回  
	if(!number_of_us)
        return;
__asm
    {
loop:
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
		NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    SUBS number_of_us, #1  //number_of_us减一
    BNE loop  //不等于0跳转到LOOP执行
    }
}
__STATIC_INLINE void sw_delay_ms(uint32_t number_of_ms)
{
    sw_delay_us(1000*number_of_ms);
}

#endif

