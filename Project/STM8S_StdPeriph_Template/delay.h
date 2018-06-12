#ifndef __DELAY_H
#define __DELAY_H

#include "stm8s.h"



typedef struct
{
  uint32_t      set_tick;
  uint32_t      overflow_tick;
}TIMEOUT_T;


//��ʱ������ʼ��
//Ϊȷ��׼ȷ��,�뱣֤ʱ��Ƶ�����Ϊ4�ı���,���8Mhz
//clk:ʱ��Ƶ��(24/16/12/8��) 
void delay_init(u8 clk);

void delay_us(u16 nus);

void delay_ms(u32 nms);

extern volatile uint32_t systick;

void os_TimeOut_init(TIMEOUT_T *t, uint32_t settick);
uint8_t os_Is_TimeOut(TIMEOUT_T *t);
void os_TimeOut_Clear(TIMEOUT_T *t);

#endif