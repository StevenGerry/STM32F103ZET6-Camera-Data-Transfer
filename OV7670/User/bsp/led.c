#include "stm32f10x.h"
#include "led.h"
#include "delay.h"

/***************************************************************************************************
 * 描  述 : 初始化单片机控制D1、D2、D3、D4的引脚PG6、PA8、PB8、PB9，并4个用户LED的初始状态设置为熄灭
 * 参  数 : 无
 * 返回值 : 无
 **************************************************************************************************/
void leds_init(void)
{
   //定义IO初始化配置结构体
   GPIO_InitTypeDef  GPIO_InitStructure;
   //打开PA  PB  PG端口时钟
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOG, ENABLE);	 
	 
	 //配置的IO是PG6
   GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;		
	 //配置为推挽输出
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	 //IO口速度为10MHz
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 //配置IO
   GPIO_Init(GPIOG, &GPIO_InitStructure);	
	
	
	//配置的IO是PA8
   GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;	
	//配置IO
   GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	//配置的IO是PB8、PB9
   GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8 | GPIO_Pin_9;	
	//配置IO
   GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
   //设置D1、D2、D3、D4初始化状态为熄灭	
	 leds_off();					
}
/***************************************************************************
 * 描  述 : 点亮开发板上的4个指示灯(D1 D2 D3 D4)
 * 参  数 : 无
 * 返回值 : 无
 ***************************************************************************/
void leds_on(void)
{
   //LED指示灯D1的引脚(PG6)输出低电平，即点亮D1
   GPIO_ResetBits(GPIOG,GPIO_Pin_6);
   //LED指示灯D2的引脚(PA8)输出低电平，即点亮D2
	 GPIO_ResetBits(GPIOA,GPIO_Pin_8);
	 //LED指示灯D3、D4的引脚(PB8 PB9)输出低电平，即点亮D3、D4
	 GPIO_ResetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_9);
}

/***************************************************************************
 * 描  述 : 熄灭开发板上的4个指示灯(D1 D2 D3 D4)
 * 参  数 : 无
 * 返回值 : 无
 ***************************************************************************/
void leds_off(void)
{
   //LED指示灯D1的引脚(PG6)输出高电平，即熄灭D1
   GPIO_SetBits(GPIOG,GPIO_Pin_6);
   //LED指示灯D1的引脚(PA8)输出高电平，即熄灭D2
	 GPIO_SetBits(GPIOA,GPIO_Pin_8);
	 //LED指示灯D1的引脚(PB8 PB9)输出高电平，即熄灭D3 D4
	 GPIO_SetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_9);
}

/***************************************************************************
 * 描  述 : 点亮一个指定的LED
 * 参  数 : [IN]led_idx:LED 对应的引脚编号
 * 返回值 : 无
 ***************************************************************************/
void led_on(uint32_t led_idx)
{
   //取得引脚的编号
	 uint16_t pin_num = (uint16_t)(led_idx&0xFFFF);
	 //区分数据端口，并驱动pin_num对应的引脚输出低电平，即点亮对应的LED
	 if(led_idx == LED_1)GPIOG->BRR = pin_num;
	 if(led_idx == LED_2)GPIOA->BRR = pin_num;
	 if((led_idx == LED_3) || (led_idx == LED_4))
	 {
		  GPIOB->BRR = pin_num;
	 }
}

/***************************************************************************
 * 描  述 : 熄灭一个指定的LED
 * 参  数 : [IN]led_idx:LED 对应的引脚编号
 * 返回值 : 无
 ***************************************************************************/
void led_off(uint32_t led_idx)
{
   //取得引脚的编号
	 uint16_t pin_num = (uint16_t)(led_idx&0xFFFF);
	 //区分数据端口，并驱动pin_num对应的引脚输出高电平，即熄灭对应的LED
	 if(led_idx == LED_1)GPIOG->BSRR = pin_num;
	 if(led_idx == LED_2)GPIOA->BSRR = pin_num;
	 if((led_idx == LED_3) || (led_idx == LED_4))
	 {
		   GPIOB->BSRR = pin_num;
	 }
}

/***************************************************************************
 * 描  述 : 翻转一个指定的LED的状态
 * 参  数 : [IN]led_idx:LED 对应的引脚编号
 * 返回值 : 无
 ***************************************************************************/
void led_toggle(uint32_t led_idx)
{
   //取得引脚的编号
	 uint16_t pin_num = (uint16_t)(led_idx&0xFFFF);
	
	 //区分数据端口，通过对ODR寄存器相应的位异或运算翻转对应的IO输出状态
	 if(led_idx == LED_1)
	 {
		 GPIOG->ODR ^= pin_num;
	 }
	 if(led_idx == LED_2)
	 {
		 GPIOA->ODR ^= pin_num;
	 }
	 if((led_idx == LED_3) || (led_idx == LED_4))
	 {
		 GPIOB->ODR ^= pin_num;
	 }
}


