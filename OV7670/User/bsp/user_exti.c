#include "stm32f10x.h"
#include "user_exti.h"
#include "ov7670.h" 
#include "delay.h"

 
uint8_t frame_flag;                                     //帧中断标记   

 /**************************************************************************************
 * 描  述 : 初始化OV7670模块VSYNC引脚为外部中断输入
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
void OV7670_PA1_Init(void)
{		
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);   //配置PA1管脚为外部中断线路用
	  
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;                    //配置为外部中断线1
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;           //配置为中断请求
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;        //输入线路上升沿为中断请求
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;	                    //使能中断
  EXTI_Init(&EXTI_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);               //NVIC_Group:先占优先级2位，从优先级2位  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;              //配置为外部中断1中断
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     //先占优先级为0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            //从优先级为1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               //使能中断通道
  NVIC_Init(&NVIC_InitStructure); 
}

/**************************************************************************************
 * 描  述 : 外部中断服务程序
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
void EXTI1_IRQHandler(void)
{		 		
 
	if(EXTI_GetITStatus(EXTI_Line1)==SET)//是8线的中断
	{     
		OV7670_WRST_L;	   //FIFO写指针复位         
		OV7670_WRST_H;	
		OV7670_WEN_H;		   //FIFO写使能，即允许写入FIFO
		frame_flag++;      //帧中断标记累加1 
	}
 
	EXTI_ClearITPendingBit(EXTI_Line1);  //清除EXTI1线路挂起位						  
} 
