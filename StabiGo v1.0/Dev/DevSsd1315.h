#ifndef _DEV_OLED_SSD1315_H
#define _DEV_OLED_SSD1315_H

#include "BaseTypes.h"
#include "DrvI2cSw.h"

/*******************************************************************************
* 宏定义 *
*******************************************************************************/
#define OLED_ADDR   0x78

#define OLED_PAGE_WIDTH ((Uint8)128u)
#define OLED_COM        ((Uint8)64u)  // OLED列数
#define OLED_PAGE       ((Uint8)8u)   // OLED页数

#define OLED_CMD    0x00u
#define OLED_DATA   0x40u

/*******************************************************************************
* 结束宏定义 *
*******************************************************************************/

/*******************************************************************************
* 结束宏定义 *
*******************************************************************************/

typedef struct Oled
{
    Uint8 ucAddr;    // OLED I2C地址
    
    Uint8 aucGDDRAM[OLED_PAGE][OLED_PAGE_WIDTH];  // OLED显示内存
    
    T_I2cSw *ptI2c;
    
    void (*ResetCtrl)(Uint8 ucValue);
    void (*DcCtrl)(Uint8 ucValue);
    
} T_Oled;


/*******************************************************************************
* 全局函数原型 *
*******************************************************************************/

void DevSsd1315Init(T_Oled *ptOled);
void DevOledRefresh(T_Oled *ptOled);
void DevOledFillAll(T_Oled *ptOled);
void DevOledClearAll(T_Oled *ptOled);
void DevOledDrawPoint(T_Oled *ptOled, Uint8 X, Uint8 Y);
void DevOledDrawChar(T_Oled *ptOled, Uint8 ucX, Uint8 ucY, Uint8 ucChar);
void DevOledDrawString(T_Oled *ptOled, Uint8 ucX, Uint8 ucY, Uint8 *pucStr);
void DevOledDrawNum(T_Oled *ptOled, Uint8 ucX, Uint8 ucY, Sint32 iNumber, Uint8 Length);
void DevOledDrawImage(T_Oled *ptOled, Uint8 ucX, Uint8 ucY, Uint8 ucImageX, Uint8 ucImageY, const Uint8 *pucImageBuf);
/*******************************************************************************
* 结束全局函数原型 *
*******************************************************************************/

#endif
