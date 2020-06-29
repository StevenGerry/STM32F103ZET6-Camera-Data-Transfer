#include "stm32f10x.h"
#include <string.h> 
#include <stdio.h>
#include <stdbool.h>

#define	FALSE					0
#define	TRUE					1

#define Buf_Max 50


#define ESP8266_RST_H   GPIO_SetBits(GPIOE,GPIO_Pin_0)
#define ESP8266_RST_L   GPIO_ResetBits(GPIOE,GPIO_Pin_0)

/****************************************************************
ATָ�
*****************************************************************/	





void esp8266_init(void);
void USART3_Init(void);
void CLR_Buf(void);
void USART3_SendByte(uint8_t byte);
bool Hand( char *a);

