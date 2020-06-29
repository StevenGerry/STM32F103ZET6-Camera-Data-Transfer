#include "stm32f10x.h"
			
#define OV7670_RRST_H   GPIO_SetBits(GPIOE,GPIO_Pin_6)
#define OV7670_RRST_L   GPIO_ResetBits(GPIOE,GPIO_Pin_6)
#define OV7670_RCK_H    GPIO_SetBits(GPIOF,GPIO_Pin_15)
#define OV7670_RCK_L    GPIO_ResetBits(GPIOF,GPIO_Pin_15)
#define OV7670_CS_H    GPIO_SetBits(GPIOG,GPIO_Pin_8)
#define OV7670_CS_L    GPIO_ResetBits(GPIOG,GPIO_Pin_8)
#define OV7670_WRST_H   GPIO_SetBits(GPIOC,GPIO_Pin_13)
#define OV7670_WRST_L   GPIO_ResetBits(GPIOC,GPIO_Pin_13)
#define OV7670_WEN_H   GPIO_SetBits(GPIOG,GPIO_Pin_5)
#define OV7670_WEN_L   GPIO_ResetBits(GPIOG,GPIO_Pin_5)

#define OV7670_DATA   GPIO_ReadInputData(GPIOC,0x00FF) 					//数据输入端口
#define CHANGE_REG_NUM 							171			                    //需要配置的寄存器总数		  
	    				 
extern u8 OV7670_Init(void);		  	   		 
extern void OV7670_Light_Mode(uint8_t mode);
extern void OV7670_Color_Saturation(uint8_t tone);
extern void OV7670_Brightness(uint8_t bright);
extern void OV7670_Contrast(uint8_t contrast);
extern void OV7670_Special_Effects(uint8_t effect);
extern void OV7670_Window_Set(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height);


