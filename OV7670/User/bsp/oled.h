#include "stm32f10x.h"
#include "delay.h"
#include "oledfont.h"

#define  OLED_DC_0   GPIO_ResetBits(GPIOB, GPIO_Pin_5) 
#define  OLED_DC_1   GPIO_SetBits(GPIOB, GPIO_Pin_5) 
#define  OLED_CS_0   GPIO_ResetBits(GPIOG, GPIO_Pin_4) 
#define  OLED_CS_1   GPIO_SetBits(GPIOG, GPIO_Pin_4) 
#define  OLED_RES_0   GPIO_ResetBits(GPIOG, GPIO_Pin_11) 
#define  OLED_RES_1   GPIO_SetBits(GPIOG, GPIO_Pin_11) 


#define OLED_WR_CMD     0
#define OLED_WR_DAT     1


#define SIZE 16

#define XLevelL        0x00
#define XLevelH        0x10
#define XLevel         ((XLevelH&0x0F)*16+XLevelL)
#define Max_Column     128
#define Max_Row        64
#define Brightness     0xCF 
#define X_WIDTH        128
#define Y_WIDTH        64


extern  void SPI_GPIO_Init(void);
extern  void SPI2_Init(void);
extern  void OLED_Init(void);
extern  void OLED_Fill(uint8_t bmp_dat);
extern  void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr);
extern  void OLED_Set_Pos(uint8_t x, uint8_t y);
extern  void LCD_P8x16Str(uint8_t x, uint8_t y,uint8_t ch[]);
extern  void LCD_P16x16Ch(uint8_t x, uint8_t y, uint8_t N);
extern  void OLED_DrawBMP(uint8_t x0, uint8_t y0,uint8_t x1, uint8_t y1,const uint8_t BMP[]);
