#include "stm32f10x.h"
#include "user_ili9341_lcd.h"
#include "ov7670.h" 
#include "user_exti.h"
#include "oled.h"
#include "delay.h"
#include "led.h"
#include "esp8266.h"

//Author Gerry Huang

extern uint8_t frame_flag;


void OV7670_tftlcd(void)
{
  uint16_t i,j;
 	uint16_t color;	 
	uint8_t re[640]={0};
	uint16_t flag=0;
	
	if(frame_flag)
	{
		Lcd_GramScan(2);
		OV7670_RRST_L;
		OV7670_RCK_L;
		OV7670_RCK_H;
		OV7670_RCK_L;
		OV7670_RRST_H;
		OV7670_RCK_H;  
    
		for(i=0;i<9600;i++)
		{
		  for(j=0;j<8;j++)
		  {
			  OV7670_RCK_L;
			  color=GPIOC->IDR&0xFF;
				re[flag++]=color;
			  OV7670_RCK_H; 
			  color<<=8;  
			  OV7670_RCK_L;
			  color|=GPIOC->IDR&0xFF;
				re[flag++]=color;
			  OV7670_RCK_H; 
        LCD_WR_Data( color );
		  }  
			if(flag==640)     
			{
				led_on(LED_4);
				USART3_SendByte(re[flag]);
				USART_SendData(USART2, re[flag]);
				flag=0;
				led_off(LED_4);
			}
    }       
		frame_flag=0;
	} 
}

int main(void)
{  
  
	leds_init();	  
	esp8266_init();	
  USART3_Init();	
  sw_delay_ms(100);
  SPI_GPIO_Init();	
  SPI2_Init();	
  OLED_Init();
  OLED_Fill(0x00);  
  LCD_Init();
	//++++++++++++++++++++++++++++++++++++++++
	while(OV7670_Init())
	{
		sw_delay_ms(20);
	} 
  sw_delay_ms(20);
	OV7670_PA1_Init();
	OV7670_Window_Set(12,176,240,320);
  OV7670_CS_L;
	//++++++++++++++++++++++++++++++++++++++++
	
	LCD_P8x16Str(1,0,"LIVE CAMERA TS");
	LCD_P8x16Str(1,2,"Status:");
	LCD_P8x16Str(1,4,"Waiting...");
	
	sw_delay_ms(1000);
	
	//LCD_P8x16Str(1,6,"DS18B20:");
	
	//++++++++++++++++++++++++++++++++++++++++
	
	while(!Hand("OK"))
	{		
		  printf("AT\r\n");
			LCD_P8x16Str(1,4,"AR");
		  sw_delay_ms(500);
	}
  CLR_Buf();
  led_on(LED_3);
  
  
	while(!(Hand("OK") | Hand("no change")))
	{		
		  printf("AT+CWMODE=3\r\n");
		  LCD_P8x16Str(1,4,"AT+CWMODE=3");
		  sw_delay_ms(500);
	}
  
  if(Hand("OK")) 
  {		
		  printf("AT+RST\r\n");
		  LCD_P8x16Str(1,4,"           ");
		  LCD_P8x16Str(1,4,"AT+RST");
		  sw_delay_ms(500);
	 }
  CLR_Buf();
  
	while(!(Hand("OK")))
	{		
		  printf("AT+CIPMUX=1\r\n");
		  LCD_P8x16Str(1,4,"AT+CIPMUX=1");
		  sw_delay_ms(500);
	 }
  CLR_Buf();
  
	while(!(Hand("OK")))
	{		
		  printf("AT+CIPSERVER=1,5000\r\n");
		  LCD_P8x16Str(1,4,"AT+CIPSERVER=1");
		  sw_delay_ms(500);
	 }
  CLR_Buf();
  
	while(!(Hand("OK")))
	{		
		  printf("AT+CIFSR\r\n");
		  LCD_P8x16Str(1,4,"AT+CIFSR");
		  sw_delay_ms(500); 
	 }
  CLR_Buf();

	 
 	while(1)
	{	
    sw_delay_ms(5);
 		OV7670_tftlcd(); 
		LCD_P8x16Str(1,6,"OV7670 Working!");
		if(Hand("ESPKLED1"))
			{
					USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
					led_on(LED_1);
					CLR_Buf();		
					USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);					
			}  
			else if(Hand("ESPGLED1")) 
			{
					USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
					led_off(LED_1);
					CLR_Buf();
					USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
			} 
			else if(Hand("ESPKLED2"))
			{
					USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
					led_on(LED_2);
					CLR_Buf();
					USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	
			} 
			else if(Hand("ESPGLED2"))
			{
					USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
					led_off(LED_2);
					CLR_Buf();
					USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);		
			} 
		LCD_DispStr(20,20,(uint8_t*)"TAG",WHITE);
	} 
	
}
