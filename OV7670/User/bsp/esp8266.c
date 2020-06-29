#include "stm32f10x.h"         
#include "esp8266.h"
#include "led.h"

volatile uint8_t Count=0;                                                      // 接收数组累加计数       
char Rx232buffer[Buf_Max];                                                     // 接收数组

/**************************************************************************************
 * 描  述 : 初始化ESP8266模块用到的引脚
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
void esp8266_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  //打开所用GPIO
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE, ENABLE); 
  
  //配置的IO是PB10，串口3的TXD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           //复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  //配置的IO是PB11，串口3的RXD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;     //浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);
     
  //配置的IO是PE0，ESP8266复位引脚RST
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;           //推挽输出
	GPIO_Init(GPIOE, &GPIO_InitStructure);
  //初始状态将ESP8266复位引脚置高
  ESP8266_RST_H;
}

/**************************************************************************************
 * 描  述 : 初始化USART3并配置USART3中断优先级
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
void USART3_Init(void)
{
	USART_InitTypeDef				USART_InitStructure;
	USART_ClockInitTypeDef  USART_ClockInitStructure;
	NVIC_InitTypeDef				NVIC_InitStructure;

  //打开串口3的时钟
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART3 , ENABLE); 
	
  //配置串口3的硬件参数
	USART_DeInit(USART3);                                            //将外设USART3寄存器重设为缺省值
	USART_InitStructure.USART_BaudRate = 115200;                      //设置串口3波特率为115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;      //设置一个帧中传输数据位 
	USART_InitStructure.USART_StopBits = USART_StopBits_1;           //定义发送的停止位数目为1
	USART_InitStructure.USART_Parity = USART_Parity_No;              //奇偶失能
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;     //硬件流控制失能
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                     //发送接收使能
	USART_Init(USART3, &USART_InitStructure);    

  USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;      //时钟低电平活动
  USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;            //设置SCLK引脚上时钟输出的极性为低电平
  USART_ClockInitStructure.USART_CPHA = USART_CPHA_1Edge;          //时钟第一个边沿进行数据捕获
  USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;  //最后一位数据的时钟脉冲不从SCLK输出
  USART_ClockInit(USART3, &USART_ClockInitStructure); 
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);                   //使能USART3接收中断
	USART_Cmd(USART3, ENABLE);                                       //使能USART3外设
	
  //配置串口3中断优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                  //NVIC_Group:先占优先级2位，从优先级2位
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	               //配置为串口3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;          //先占优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	             //从优先级为3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                  //使能中断通道
	NVIC_Init(&NVIC_InitStructure);
}

/**************************************************************************************
 * 描  述 : USART3发送函数
 * 入  参 : 单个字符
 * 返回值 : 无
 **************************************************************************************/
void USART3_SendByte(uint8_t byte)
{
	while(USART_GetFlagStatus(USART3,USART_FLAG_TC) != SET);
	USART_SendData(USART3, byte);
}

/**************************************************************************************
 * 描  述 : 重定向c库函数printf到USART3
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到USART3 */
		USART_SendData(USART3, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

/**************************************************************************************
 * 描  述 : 重定向c库函数scanf到USART3
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
int fgetc(FILE *f)
{
		/* 等待串口3输入数据 */
		while (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART3);
}

/**************************************************************************************
 * 描  述 : 握手成功与否函数
 * 入  参 : char *a待对比字符串
 * 返回值 : 无
 **************************************************************************************/
bool Hand( char *a)
{ 
  if(strstr(Rx232buffer,a)!=NULL)    //判断指针a中的字符串是否是Rx232buffer数组中字符串的子串
	   return true;
	else
		 return false;
}

/**************************************************************************************
 * 描  述 : 清空接收数组数据
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
void CLR_Buf(void)
{
	uint8_t k;
  for(k=0;k<Buf_Max;k++)      
  {
			Rx232buffer[k] = 0;
	}      
  Count = 0;                                         //接收数组累加计数清零，即下次接收从头开始
}

/**************************************************************************************
 * 描  述 : USART3全局中断服务
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
void USART3_IRQHandler(void)
{  
  if(USART_GetITStatus(USART3,USART_IT_RXNE)!= RESET)       //接收中断
  {  
    Rx232buffer[Count]= USART_ReceiveData(USART3);          //通过外设USART3接收数据
		Count++;                                                //不断累加接收数据                          
	  if(Count>Buf_Max)                                       //接收数大于定义接收数组最大个数时，从头开始放置数据
		{
			 Count = 0;
		} 
    USART_ClearITPendingBit(USART3, USART_IT_RXNE);         //清除USART3的中断待处理位
  }  
}

