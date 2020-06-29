#include "stm32f10x.h"
#include "oled.h"
#include "delay.h"


/**************************************************************************************
 * 描  述 : 初始化OLED所用到的IO口
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
void SPI_GPIO_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  //打开所用GPIO的时钟
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOG , ENABLE); 
  
  //配置的IO是PB5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;           //推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  //配置的IO是PB13 PB15，SPI的CLK MOSI
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           //复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  //配置的IO是PG4 PG11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_11;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;           //推挽输出
	GPIO_Init(GPIOG, &GPIO_InitStructure);
}

/***************************************************************************
 * 描  述 : SPI2读写数据函数，读写一个字节
 * 入  参 : Dat：写入的数据 mode：=0：写入命令，=1：写入数据
 * 返回值 : 读取的数据
 **************************************************************************/
uint8_t SPI2_ReadWriteByte(uint8_t Dat,uint8_t mode)                                       
{		
	uint8_t retry=0;	

  if(mode == OLED_WR_CMD) 
    OLED_DC_0;
  else 
    OLED_DC_1;
  
	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)      //发送缓存标志位为空
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI_I2S_SendData(SPI2, Dat);                                        //通过外设SPI2发送一个数据
	retry=0;
	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)     //接收缓存标志位不为空
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI2);                                   //通过SPI2返回接收数据				    
}

/***************************************************************************
 * 描  述 : 配置SPI2总线
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************/
void SPI2_Init(void)
{	 
	SPI_InitTypeDef  SPI_InitStructure;
    
  // Enable SPI2 clocks
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE); //SPI2时钟使能

	SPI_Cmd(SPI2, DISABLE); 
	/* SPI2 configuration */                                              //初始化SPI结构体
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;    //SPI设置为双线全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		                      //设置SPI为主模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		                  //SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		                        //SPI时钟空闲时为低电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	                        //第一个时钟沿开始采样数据
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		                          //NSS信号由软件（使用SSI位）管理
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;   //SPI2波特率预分频值为32
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	                  //数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	                            //CRC值计算的多项式

	SPI_Init(SPI2, &SPI_InitStructure);      //根据SPI_InitStruct中指定的参数初始化外设SPI2寄存器
	SPI_Cmd(SPI2, ENABLE);                   //使能SPI2外设
	SPI2_ReadWriteByte(0xff,0);              //启动传输	
} 
 
/******************************************************************************
 * 描  述 : 设置坐标
 * 入  参 : x：x坐标；y：y坐标
 * 返回值 : 无
 ******************************************************************************/
void OLED_Set_Pos(uint8_t x, uint8_t y) 
{ 
    SPI2_ReadWriteByte((0xb0+y),OLED_WR_CMD);
    SPI2_ReadWriteByte(((x&0xf0)>>4)|0x10,OLED_WR_CMD);
    SPI2_ReadWriteByte((x&0x0f)|0x01,OLED_WR_CMD); 
} 

/******************************************************************************
 * 描  述 : OLED清屏函数
 * 入  参 : 无
 * 返回值 : 无
 ******************************************************************************/
void OLED_Fill(uint8_t dat) 
{
    uint8_t y,x;
    for(y=0;y<8;y++)
    {
        SPI2_ReadWriteByte(0xb0+y,OLED_WR_CMD);//设置页地址（0~7）
        SPI2_ReadWriteByte(0x02,OLED_WR_CMD); //设置显示位置—列低地址
        SPI2_ReadWriteByte(0x10,OLED_WR_CMD); //设置显示位置—列高地址
        for(x=0; x<X_WIDTH; x++)
            SPI2_ReadWriteByte(dat,OLED_WR_DAT);
    }
}

/******************************************************************************
 * 描  述 : 指定位置显示一个字符
 * 入  参 : x:列0~127；y:页地址0~7；
 * 返回值 : 无
 ******************************************************************************/
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr)
{      	
  uint8_t c=0,i=0;	
  
  c = chr-' ';//得到偏移后的值			
  if(x > Max_Column-1)
  {
    x = 0;
    y = y+2;
  }

  OLED_Set_Pos(x,y);	
  for(i=0; i<8; i++) SPI2_ReadWriteByte(codeF8X16[c*16+i],OLED_WR_DAT); 
  OLED_Set_Pos(x,y+1);
  for(i=0;i<8;i++)SPI2_ReadWriteByte(codeF8X16[c*16+i+8],OLED_WR_DAT);
}

/******************************************************************************
 * 描  述 : 显示8*16一组标准ASCII字符串
 * 入  参 : x:列0~127；y:页地址0~7；
 * 返回值 : 无
 ******************************************************************************/
void LCD_P8x16Str(uint8_t x, uint8_t y,uint8_t ch[])
{
  uint8_t c=0,i=0,j=0;
    
  while (ch[j] != '\0')
  {    
    c = ch[j]-32;
    if(x>120){x=0;y++;}
    OLED_Set_Pos(x,y);    
    for(i=0; i<8; i++)SPI2_ReadWriteByte(codeF8X16[c*16+i],OLED_WR_DAT);          
    OLED_Set_Pos(x,y+1);    
    for(i=0;i<8;i++) SPI2_ReadWriteByte(codeF8X16[c*16+i+8],OLED_WR_DAT);        
    x += 8;
    j++;
  }
}

/******************************************************************************
 * 描  述 : 显示汉字
 * 入  参 : x:列0~127；y:页地址0~7；
 * 返回值 : 无
 ******************************************************************************/
void LCD_P16x16Ch(uint8_t x, uint8_t y, uint8_t N)
{
    uint8_t wm=0;
    unsigned int adder=32*N;        
    OLED_Set_Pos(x , y);
    for(wm = 0;wm < 16;wm++)               
    {
        SPI2_ReadWriteByte(codeF16x16[adder],OLED_WR_DAT);    
        adder += 1;
    }      
    OLED_Set_Pos(x,y + 1); 
    for(wm = 0;wm < 16;wm++)         
    {
        SPI2_ReadWriteByte(codeF16x16[adder],OLED_WR_DAT);
        adder += 1;
    }           
}

/******************************************************************************
 * 描  述 : 显示BMP图片128×64
 * 入  参 : 起始点坐标(x,y),x的范围0～127，y为页的范围0～7
 * 返回值 : 无
 ******************************************************************************/
void OLED_DrawBMP(uint8_t x0, uint8_t y0,uint8_t x1, uint8_t y1,const uint8_t BMP[])
{ 	
  uint16_t j=0;
  uint8_t x,y;
  
  if((y1 % 8) == 0) y = y1/8;      
  else y = y1/8+1;
  for(y=y0;y<y1;y++)
  {
    OLED_Set_Pos(x0,y);
    for(x=x0; x<x1; x++)
    {      
      SPI2_ReadWriteByte(BMP[j++],OLED_WR_DAT);	    	
    }
  }
} 

/******************************************************************************
 * 描  述 : OLED初始化
 * 入  参 : 无
 * 返回值 : 无
 ******************************************************************************/
void OLED_Init(void)     
{    
		OLED_RES_0;    //OLED复位引脚拉低
    OLED_CS_1;     //OLED使能引脚置高电平
	  sw_delay_ms(10);       //该延时必不可少,是给初始化足够的时间
	  OLED_RES_1;    //OLED复位引脚拉高
    OLED_CS_0;     //OLED使能引脚置低电平
    sw_delay_ms(200);

    SPI2_ReadWriteByte(0xae,OLED_WR_CMD);//--turn off oled panel
    SPI2_ReadWriteByte(0x00,OLED_WR_CMD);//---set low column address
    SPI2_ReadWriteByte(0x10,OLED_WR_CMD);//---set high column address
    SPI2_ReadWriteByte(0x40,OLED_WR_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    SPI2_ReadWriteByte(0x81,OLED_WR_CMD);//--set contrast control register
    SPI2_ReadWriteByte(0xcf,OLED_WR_CMD); // Set SEG Output Current Brightness
	
    SPI2_ReadWriteByte(0xa1,OLED_WR_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    SPI2_ReadWriteByte(0xc8,OLED_WR_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    SPI2_ReadWriteByte(0xa6,OLED_WR_CMD);//--set normal display
    SPI2_ReadWriteByte(0xa8,OLED_WR_CMD);//--set multiplex ratio(1 to 64)
    SPI2_ReadWriteByte(0x3f,OLED_WR_CMD);//--1/64 duty
	
    SPI2_ReadWriteByte(0xd3,OLED_WR_CMD);//-set display offset    Shift Mapping RAM Counter (0x00~0x3F)
    SPI2_ReadWriteByte(0x00,OLED_WR_CMD);//-not offset
		
    SPI2_ReadWriteByte(0xd5,OLED_WR_CMD);//--set display clock divide ratio/oscillator frequency
    SPI2_ReadWriteByte(0x80,OLED_WR_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
    SPI2_ReadWriteByte(0xd9,OLED_WR_CMD);//--set pre-charge period
		
    SPI2_ReadWriteByte(0xf1,OLED_WR_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
		//OLED_WrByte(0x22,OLED_WR_CMD);
    SPI2_ReadWriteByte(0xda,OLED_WR_CMD);//--set com pins hardware configuration
    SPI2_ReadWriteByte(0x12,OLED_WR_CMD);
    SPI2_ReadWriteByte(0xdb,OLED_WR_CMD);//--set vcomh
		
    SPI2_ReadWriteByte(0x40,OLED_WR_CMD);//Set VCOM Deselect Level
		
    SPI2_ReadWriteByte(0x20,OLED_WR_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
    SPI2_ReadWriteByte(0x02,OLED_WR_CMD);//
    SPI2_ReadWriteByte(0x8d,OLED_WR_CMD);//--set Charge Pump enable/disable
    SPI2_ReadWriteByte(0x14,OLED_WR_CMD);//--set(0x10) disable
    SPI2_ReadWriteByte(0xa4,OLED_WR_CMD);// Disable Entire Display On (0xa4/0xa5)
    SPI2_ReadWriteByte(0xa6,OLED_WR_CMD);// Disable Inverse Display On (0xa6/a7) 
    SPI2_ReadWriteByte(0xaf,OLED_WR_CMD);//--turn on oled panel
    OLED_Fill(0xff);  //初始清屏
    OLED_Set_Pos(0,0);     
}


/*********************************END FILE*************************************/
