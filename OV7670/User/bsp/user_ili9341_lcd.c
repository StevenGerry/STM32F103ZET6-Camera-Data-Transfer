#include "stm32f10x.h"
#include "user_ili9341_lcd.h"
#include "ascii.h"	
#include "delay.h"

#define DEBUG_DELAY()   sw_delay_us(1)

/**************************************************************************************
 * 描  述 : 初始化控制TFTLCD的IO
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
void LCD_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* 使能FSMC时钟*/
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
    
    /* 使能FSMC对应相应管脚时钟*/
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE 
	                          | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOG  , ENABLE);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    /* 配置LCD背光控制管脚*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    /* 配置LCD复位控制管脚*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ; 	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_Init(GPIOD, &GPIO_InitStructure);  		   
    
    /* 配置FSMC相对应的数据线,FSMC-D0~D15: PD 14 15 0 1,PE 7 8 9 10 11 12 13 14 15,PD 8 9 10*/	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | 
                                  GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                  GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure); 
    
		/* 配置FSMC相对应的控制线
		 * PD4-FSMC_NOE   :LCD-RD
		 * PD5-FSMC_NWE   :LCD-WR
		 * PG12-FSMC_NE4  :LCD-CS
		 * PG0-FSMC_A10   :LCD-DC
		*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; 
    GPIO_Init(GPIOG, &GPIO_InitStructure);  
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
    GPIO_Init(GPIOG, &GPIO_InitStructure); 
    
    /* 开背光 */
		GPIO_ResetBits(GPIOD, GPIO_Pin_13);  
}

/**************************************************************************************
 * 描  述 : TFTLCD  FSMC 模式配置
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
void LCD_FSMC_Config(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  readWriteTiming; 
	FSMC_NORSRAMTimingInitTypeDef  writeTiming;
   
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);	//使能FSMC时钟  
    
	readWriteTiming.FSMC_AddressSetupTime = 0x01;	 //地址建立时间（ADDSET）为2个HCLK 1/36M=27ns
  readWriteTiming.FSMC_AddressHoldTime = 0x00;	 //地址保持时间（ADDHLD）模式A未用到	
  readWriteTiming.FSMC_DataSetupTime = 0x0f;		 // 数据保存时间为16个HCLK,因为液晶驱动IC的读数据的时候，速度不能太快，尤其对1289这个IC。
  readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
  readWriteTiming.FSMC_CLKDivision = 0x00;
  readWriteTiming.FSMC_DataLatency = 0x00;
  readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 
    

	writeTiming.FSMC_AddressSetupTime = 0x00;	 //地址建立时间（ADDSET）为1个HCLK  
  writeTiming.FSMC_AddressHoldTime = 0x00;	 //地址保持时间（A		
  writeTiming.FSMC_DataSetupTime = 0x03;		 ////数据保存时间为4个HCLK	
  writeTiming.FSMC_BusTurnAroundDuration = 0x00;
  writeTiming.FSMC_CLKDivision = 0x00;
  writeTiming.FSMC_DataLatency = 0x00;
  writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 
    
 
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;//  这里我们使用NE4 ，也就对应BTCR[6],[7]。
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; // 不复用数据地址
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;

  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//存储器数据宽度为16bit   
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable; 
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable; 
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;   
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	//  存储器写使能
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;   
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; // 读写使用不同的时序
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable; 
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming; //读写时序
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;  //写时序

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //初始化FSMC配置

 	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  // 使能BANK1 
  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
    
  /* 使能 FSMC Bank1_SRAM Bank */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  
}

/**************************************************************************************
 * 描  述 : TFTLCD 复位
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
void LCD_Rst(void)
{			
		GPIO_ResetBits(GPIOD, GPIO_Pin_11);	 //低电平复位
    sw_delay_ms(1000); 					   
    GPIO_SetBits(GPIOD, GPIO_Pin_11);		 	 
    sw_delay_ms(30);
}

/**************************************************************************************
 * 描  述 : TFTLCD 初始化寄存器
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
void LCD_REG_Config(void)
{
    LCD_ILI9341_CMD(0xCF);  
		LCD_ILI9341_Parameter(0x00); 
		LCD_ILI9341_Parameter(0xC1); 
		LCD_ILI9341_Parameter(0X30); 
    
		LCD_ILI9341_CMD(0xED);  
		LCD_ILI9341_Parameter(0x64); 
		LCD_ILI9341_Parameter(0x03); 
		LCD_ILI9341_Parameter(0X12); 
		LCD_ILI9341_Parameter(0X81);
    
		LCD_ILI9341_CMD(0xE8);  
		LCD_ILI9341_Parameter(0x85); 
		LCD_ILI9341_Parameter(0x10); 
		LCD_ILI9341_Parameter(0x7A);
    
		LCD_ILI9341_CMD(0xCB);  
		LCD_ILI9341_Parameter(0x39); 
		LCD_ILI9341_Parameter(0x2C); 
		LCD_ILI9341_Parameter(0x00); 
		LCD_ILI9341_Parameter(0x34); 
		LCD_ILI9341_Parameter(0x02); 
    
		LCD_ILI9341_CMD(0xF7);  
		LCD_ILI9341_Parameter(0x20);
    
		LCD_ILI9341_CMD(0xEA);  
		LCD_ILI9341_Parameter(0x00); 
		LCD_ILI9341_Parameter(0x00);
    
		LCD_ILI9341_CMD(0xC0);    //Power control 
		LCD_ILI9341_Parameter(0x1B);   //VRH[5:0] 
    
		LCD_ILI9341_CMD(0xC1);    //Power control 
		LCD_ILI9341_Parameter(0x01);   //SAP[2:0];BT[3:0] 
    
		LCD_ILI9341_CMD(0xC5);    //VCM control 
		LCD_ILI9341_Parameter(0x30); 	 //3F
		LCD_ILI9341_Parameter(0x30); 	 //3C
    
		LCD_ILI9341_CMD(0xC7);    //VCM control2 
		LCD_ILI9341_Parameter(0XB7); 
    
		LCD_ILI9341_CMD(0x36);    // Memory Access Control 
		LCD_ILI9341_Parameter(0x48); 
    
		LCD_ILI9341_CMD(0x3A);   
		LCD_ILI9341_Parameter(0x55); 
    
		LCD_ILI9341_CMD(0xB1);   
		LCD_ILI9341_Parameter(0x00);   
		LCD_ILI9341_Parameter(0x1A); 
    
		LCD_ILI9341_CMD(0xB6);    // Display Function Control 
		LCD_ILI9341_Parameter(0x0A); 
		LCD_ILI9341_Parameter(0xA2); 
    
		LCD_ILI9341_CMD(0xF2);    // 3Gamma Function Disable 
		LCD_ILI9341_Parameter(0x00); 
    
		LCD_ILI9341_CMD(0x26);    //Gamma curve selected 
		LCD_ILI9341_Parameter(0x01); 
    
		LCD_ILI9341_CMD(0xE0);    //Set Gamma 
		LCD_ILI9341_Parameter(0x0F); 
		LCD_ILI9341_Parameter(0x2A); 
		LCD_ILI9341_Parameter(0x28); 
		LCD_ILI9341_Parameter(0x08); 
		LCD_ILI9341_Parameter(0x0E); 
		LCD_ILI9341_Parameter(0x08); 
		LCD_ILI9341_Parameter(0x54); 
		LCD_ILI9341_Parameter(0XA9); 
		LCD_ILI9341_Parameter(0x43); 
		LCD_ILI9341_Parameter(0x0A); 
		LCD_ILI9341_Parameter(0x0F); 
		LCD_ILI9341_Parameter(0x00); 
		LCD_ILI9341_Parameter(0x00); 
		LCD_ILI9341_Parameter(0x00); 
		LCD_ILI9341_Parameter(0x00); 
    
		LCD_ILI9341_CMD(0xE1);    //Set Gamma 
		LCD_ILI9341_Parameter(0x00); 
		LCD_ILI9341_Parameter(0x15); 
		LCD_ILI9341_Parameter(0x17); 
		LCD_ILI9341_Parameter(0x07); 
		LCD_ILI9341_Parameter(0x11); 
		LCD_ILI9341_Parameter(0x06); 
		LCD_ILI9341_Parameter(0x2B); 
		LCD_ILI9341_Parameter(0x56); 
		LCD_ILI9341_Parameter(0x3C); 
		LCD_ILI9341_Parameter(0x05); 
		LCD_ILI9341_Parameter(0x10); 
		LCD_ILI9341_Parameter(0x0F); 
		LCD_ILI9341_Parameter(0x3F); 
		LCD_ILI9341_Parameter(0x3F); 
		LCD_ILI9341_Parameter(0x0F);
    
		LCD_ILI9341_CMD(0x2B); 
		LCD_ILI9341_Parameter(0x00);
		LCD_ILI9341_Parameter(0x00);
		LCD_ILI9341_Parameter(0x01);
		LCD_ILI9341_Parameter(0x3f);
    
		LCD_ILI9341_CMD(0x2A); 
		LCD_ILI9341_Parameter(0x00);
		LCD_ILI9341_Parameter(0x00);
		LCD_ILI9341_Parameter(0x00);
		LCD_ILI9341_Parameter(0xef);
    
		LCD_ILI9341_CMD(0x11); //Exit Sleep
		sw_delay_ms(120);
    
		LCD_ILI9341_CMD(0x29); //display on	
  
}

/**************************************************************************************
 * 描  述 : TFTLCD初始化
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
void LCD_Init(void)
{
	LCD_GPIO_Config();
  LCD_Rst();
	LCD_FSMC_Config();	
	LCD_REG_Config();
}

/**************************************************************************************
 * 描  述 : 设置TFT屏GRAM的扫描方向 
 * 入  参 : option  扫描模式设置
 * 返回值 : 无
 **************************************************************************************/
void Lcd_GramScan( uint16_t option )
{	
	switch(option)
	{
		case 1:
		{/* 左上角->右下脚      显示中英文时用的是这种模式 */
			LCD_ILI9341_CMD(0x36); 
			LCD_ILI9341_Parameter(0xC8);   
			LCD_ILI9341_CMD(0x2A); 
			LCD_ILI9341_Parameter(0x00);	/* x start */	
			LCD_ILI9341_Parameter(0x00);
			LCD_ILI9341_Parameter(0x00);  /* x end */	
			LCD_ILI9341_Parameter(0xEF);

			LCD_ILI9341_CMD(0x2B); 
			LCD_ILI9341_Parameter(0x00);	/* y start */  
			LCD_ILI9341_Parameter(0x00);
			LCD_ILI9341_Parameter(0x01);	/* y end */   
			LCD_ILI9341_Parameter(0x3F);
					
		}break;
		case 2:
		{/* 左下角->右上角      显示摄像头图像时用的是这种模式 */	
			LCD_ILI9341_CMD(0x36); 
			LCD_ILI9341_Parameter(0x68);	
			LCD_ILI9341_CMD(0x2A); 
			LCD_ILI9341_Parameter(0x00);
			LCD_ILI9341_Parameter(0x00);
			LCD_ILI9341_Parameter(0x01);
			LCD_ILI9341_Parameter(0x3F);	

			LCD_ILI9341_CMD(0x2B); 
			LCD_ILI9341_Parameter(0x00);
			LCD_ILI9341_Parameter(0x00);
			LCD_ILI9341_Parameter(0x00);
			LCD_ILI9341_Parameter(0xEF);			
		}break;
		case 3:
		{/* 右下角->左上角      显示BMP图片时用的是这种模式 */
			LCD_ILI9341_CMD(0x36); 
			LCD_ILI9341_Parameter(0x28);	
			LCD_ILI9341_CMD(0x2A); 
			LCD_ILI9341_Parameter(0x00);
			LCD_ILI9341_Parameter(0x00);
			LCD_ILI9341_Parameter(0x01);
			LCD_ILI9341_Parameter(0x3F);	

			LCD_ILI9341_CMD(0x2B); 
			LCD_ILI9341_Parameter(0x00);
			LCD_ILI9341_Parameter(0x00);
			LCD_ILI9341_Parameter(0x00);
			LCD_ILI9341_Parameter(0xEF);			
		}break;
	}
		
	/* write gram start */
	LCD_ILI9341_CMD(0x2C);
}

void LCD_Clear(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
	uint32_t i = 0;
	
	/* column address control set */
	LCD_ILI9341_CMD(0X2A);
	LCD_ILI9341_Parameter( x >> 8 );	 /* 先高8位，然后低8位 */
	LCD_ILI9341_Parameter( x & 0xff );	         /* column start   */ 
	LCD_ILI9341_Parameter( (x+width-1) >> 8 );   /* column end   */
	LCD_ILI9341_Parameter( (x+width-1) & 0xff );
	
	/* page address control set */	
  LCD_ILI9341_CMD(0X2B); 			     
	LCD_ILI9341_Parameter( y >> 8 );			/* page start   */
	LCD_ILI9341_Parameter( y & 0xff );
	LCD_ILI9341_Parameter( (y+height-1) >> 8);  /* page end     */
	LCD_ILI9341_Parameter( (y+height-1) & 0xff);
	
	/* memory write */
	LCD_ILI9341_CMD(0x2C);	
		
	for( i=0; i < width*height; i++ )
	{
		LCD_WR_Data( color );
	}	
}


void LCD_SetCursor(uint16_t x, uint16_t y)	
{	
	LCD_ILI9341_CMD(0x2A); 				 /* 设置X坐标 */
	LCD_ILI9341_Parameter(x>>8);	 /* 先高8位，然后低8位 */
	LCD_ILI9341_Parameter(x&0xff);	 /* 设置起始点和结束点*/

  LCD_ILI9341_CMD(0x2B); 			     /* 设置Y坐标*/
	LCD_ILI9341_Parameter(y>>8);
	LCD_ILI9341_Parameter(y&0xff);
     
}

//  _ _ _ _ _ _
// |           |
// |           |
// |           |
// |           |
// |           |
// |           |
// |           |
// |           |
// |           |
// |           |
// |           |
// |           |
//  - - - - - -
void LCD_OpenWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{	
	LCD_ILI9341_CMD(0x2A); 				 /* 设置X坐标 */
	LCD_ILI9341_Parameter( x >> 8 );	 /* 先高8位，然后低8位 */
	LCD_ILI9341_Parameter( x & 0xff );	 /* 设置起始点和结束点*/
	LCD_ILI9341_Parameter( (x+width-1) >> 8 );
	LCD_ILI9341_Parameter( (x+width-1) & 0xff );

	LCD_ILI9341_CMD(0x2B); 			     /* 设置Y坐标*/
	LCD_ILI9341_Parameter( y >> 8 );
	LCD_ILI9341_Parameter( y & 0xff );
	LCD_ILI9341_Parameter( (y+height-1) >> 8);
	LCD_ILI9341_Parameter( (y+height-1) & 0xff);	
		
	/* write gram start */
	LCD_ILI9341_CMD(0x2C);
}

void LCD_SetPoint(uint16_t x , uint16_t y , uint16_t color)	
{	
	LCD_SetCursor(x, y);
	LCD_ILI9341_CMD(0x2C);	         /* 写数据 */
	LCD_WR_Data(color);
}

uint16_t LCD_RD_data(void)	
{	
	uint16_t R=0, G=0, B=0 ;

	R = *(__IO uint16_t *)Bank1_LCD_D; 	  /*FIRST READ OUT DUMMY DATA*/
	R = *(__IO uint16_t *)Bank1_LCD_D;  	/*READ OUT RED DATA  */
	B = *(__IO uint16_t *)Bank1_LCD_D;  	/*READ OUT BLACK DATA*/
	G = *(__IO uint16_t *)Bank1_LCD_D;  	/*READ OUT GREEN DATA*/
	
    return (((R>>11)<<11) | ((G>>10)<<5) | (B>>11));
}

uint16_t LCD_GetPoint(uint16_t x , uint16_t y)
{ 
	uint16_t temp;

	LCD_SetCursor(x, y);
	LCD_ILI9341_CMD(0x2E);         /* 读数据 */
	temp=LCD_RD_data();
	return (temp);
}


 
 #define COLUMN_WINDOW 230
 /**************************************************************************************
 * 描  述 : TFTLCD显示单个字符函数
 * 入  参 : x 行, y 列, ascii 待显示字符, color 显示内容的颜色
 * 返回值 : 无
 **************************************************************************************/
void LCD_DispChar(uint16_t x, uint16_t y, uint8_t ascii, uint16_t color)
{
	uint16_t page, column, temp, i;
	i = ascii - ' ';
	
	LCD_OpenWindow(x, y, STR_WIDTH, STR_HEIGHT);
	LCD_ILI9341_CMD(0X2C);	
	
	for( page=0; page < STR_HEIGHT; page++ )
	{
		temp = asc2_1206[i][page];
		for( column=0; column < STR_WIDTH; column++ )
		{
			if( temp & 0x01 )
			{
				LCD_WR_Data( color );
			}
			else
			{
				LCD_WR_Data( BACKGROUND );								
			}
			temp >>= 1;		
		}/* 一行写完 */
	}/* 全部写完 */
}

/**************************************************************************************
 * 描  述 : TFTLCD显示字符串函数
 * 入  参 : x 行, y 列, *pstr 待显示字符串, color 显示内容的颜色
 * 返回值 : 无
 **************************************************************************************/
void LCD_DispStr(uint16_t x, uint16_t y, uint8_t *pstr, uint16_t color)
{
	while( *pstr != '\0' )
	{
		if( x > (COLUMN_WINDOW-STR_WIDTH) )
		{
			x = 20;
			y += STR_HEIGHT;
		}
		if( y > (PAGE-STR_HEIGHT) )
		{
			x = 0;
			y = 0;
		}
		LCD_DispChar(x, y, *pstr, color);
		x += STR_WIDTH;
		pstr++;
	}
}

/**************************************************************************************
 * 描  述 : TFTLCD显示单个数字函数
 * 入  参 : x 行, y 列, num 待显示数字, color 显示内容的颜色
 * 返回值 : 无
 **************************************************************************************/
void LCD_DisNum(uint16_t x, uint16_t y, uint32_t num, uint16_t color)
{
	uint32_t length = 0, temp = 0;
	temp = num;
	
	if( temp == 0 )
	{
		LCD_DispChar(x, y, '0', color);
		return;
	}
	
	while( temp )
	{// 得到num的长度
		temp /= 10;
		length ++;
	}
	
	while( num )
	{
		if( x > (COLUMN_WINDOW-STR_WIDTH) )
		{
			x = 20;
			y += STR_HEIGHT;
		}
		if( y > (PAGE-STR_HEIGHT) )
		{
			x = 0;
			y = 0;
		}
		/* 从个位开始显示 */
		LCD_DispChar((x+STR_WIDTH*(length--)-STR_WIDTH), y, (num%10)+'0', color);
		num /= 10;
	}	
}


__inline void ILI9341_Write_Data ( uint16_t usData )
{
	* ( __IO uint16_t * ) ( Bank1_LCD_D ) = usData;
	
}

/********************************************END FILE********************/

