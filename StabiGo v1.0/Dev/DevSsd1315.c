#include "DevSsd1315.h"
#include "FontLib.h"

/*******************************************************************************
* 函数名称： DevOledSendCmd()
* 功能描述： ssd1315 发送命令
* 输入参数： ptOled:  oled结构体指针
            ucTemp: 要发送的数据
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
*******************************************************************************/
static void DevOledSendCmd(T_Oled *ptOled, Uint8 ucTemp)
{
    DrvI2cSwStart(ptOled->ptI2c);
    DrvI2cSwSendByte(ptOled->ptI2c, ptOled->ucAddr);
    DrvI2cSwWaitAck(ptOled->ptI2c);
    DrvI2cSwSendByte(ptOled->ptI2c, OLED_CMD);
    DrvI2cSwWaitAck(ptOled->ptI2c);
    DrvI2cSwSendByte(ptOled->ptI2c, ucTemp);
    DrvI2cSwWaitAck(ptOled->ptI2c);
    DrvI2cSwStop(ptOled->ptI2c);
}

/*******************************************************************************
* 函数名称： DevOledSendData()
* 功能描述： ssd1315 发送数据
* 输入参数： ptOled:  oled结构体指针
            pucBuf: 要发送的数据缓冲
            ucLen：要发送的数据长度
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
*******************************************************************************/
static void DevOledSendData(T_Oled *ptOled, Uint8 *pucBuf,Uint8 ucLen)
{
    DrvI2cSwStart(ptOled->ptI2c);
    DrvI2cSwSendByte(ptOled->ptI2c, ptOled->ucAddr);
    DrvI2cSwWaitAck(ptOled->ptI2c);
    DrvI2cSwSendByte(ptOled->ptI2c, OLED_DATA);
    DrvI2cSwWaitAck(ptOled->ptI2c);
    
    for(Uint8 i = 0; i < ucLen; i++)
    {
        DrvI2cSwSendByte(ptOled->ptI2c, pucBuf[i]);
        DrvI2cSwWaitAck(ptOled->ptI2c);        
    }
    
    DrvI2cSwStop(ptOled->ptI2c);
}

/*******************************************************************************
* 函数名称： DevOledRefresh()
* 功能描述： ssd1315 oled 刷新
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
*******************************************************************************/
void DevOledRefresh(T_Oled *ptOled)
{
    Uint8 ucPage = 0;
    
    for(ucPage = 0; ucPage < OLED_PAGE; ucPage++)
    {
        DevOledSendCmd(ptOled, 0xB0 + ucPage);
        DevOledSendCmd(ptOled, 0x00);
        DevOledSendCmd(ptOled, 0x10);
        
        DevOledSendData(ptOled, ptOled->aucGDDRAM[ucPage], OLED_PAGE_WIDTH);
    }
}

/*******************************************************************************
* 函数名称： DevOledFillAll()
* 功能描述： ssd1315 oled 填充屏幕
* 输入参数： ptOled:  oled结构体指针
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
*******************************************************************************/
void DevOledFillAll(T_Oled *ptOled)
{
    Uint8 ucPage = 0u;
    
    for(ucPage = 0u; ucPage < OLED_PAGE; ucPage++)
    {
        memset(ptOled->aucGDDRAM[ucPage], 0xFF, OLED_PAGE_WIDTH);
    }    
}

/*******************************************************************************
* 函数名称： DevOledClearAll()
* 功能描述： ssd1315 oled 清屏
* 输入参数： ptOled:  oled结构体指针
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
*******************************************************************************/
void DevOledClearAll(T_Oled *ptOled)
{
    Uint8 ucPage = 0u;
    
    for(ucPage = 0u; ucPage < OLED_PAGE; ucPage++)
    {
        memset(ptOled->aucGDDRAM[ucPage], 0u, OLED_PAGE_WIDTH);
    }    
}

/*******************************************************************************
* 函数名称： DevOledClearArea()
* 功能描述： ssd1315 oled 清楚指定区域
* 输入参数： ptOled:  oled结构体指针
            ucX: X偏移
            ucY: Y偏移
            ucImageX: 图片X方向大小
            ucImageY：图片Y方向大小
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
*******************************************************************************/
void DevOledClearArea(T_Oled *ptOled, Uint8 ucX, Uint8 ucY, Uint8 ucImageX, Uint8 ucImageY)
{
	Uint8 ucXmax = MIN(ucX + ucImageX, OLED_PAGE_WIDTH);
	Uint8 ucYmax = MIN(ucY + ucImageY, OLED_COM);
	
	for (Uint8 i = ucX; i < ucXmax; i++)
	{
		for (Uint8 j = ucY; j < ucYmax; j ++)
		{
			ptOled->aucGDDRAM[j / 8][i] &= ~(0x01 << (j % 8));
		}
	}	
}

/*******************************************************************************
* 函数名称： DevOledDrawPoint()
* 功能描述： ssd1315 oled 画点函数
* 输入参数： ptOled:  oled结构体指针
            ucX: X偏移
            ucY: Y偏移
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 
(0,0) ______________ X(0-127)
	 |
	 |
  Y(0-63)
*******************************************************************************/
void DevOledDrawPoint(T_Oled *ptOled, Uint8 ucX, Uint8 ucY)
{
	if ((ucX <= 127u) && (ucY <= 63u))		
	{		
		ptOled->aucGDDRAM[ucY / 8u][ucX] |= 0x01 << (ucY % 8u);  
	}
}

/*******************************************************************************
* 函数名称： DevOledDrawImage()
* 功能描述： ssd1315 oled 画图函数
* 输入参数： ptOled:  oled结构体指针
            ucX: X偏移
            ucY: Y偏移
            ucImageX: 图片X方向大小
            ucImageY：图片Y方向大小
            pucImageBuf：图片buf
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 
(0,0) ______________ X(0-127)
	 |
	 |
  Y(0-63)
*******************************************************************************/
void DevOledDrawImage(T_Oled *ptOled, Uint8 ucX, Uint8 ucY, Uint8 ucImageX, Uint8 ucImageY, const Uint8 *pucImageBuf)
{
    if (((ucX > 127u) && (ucY > 63u)) || (((ucX + ucImageX) > 127u) && ((ucY + ucImageY) > 63u)))
    {
        return;
    }
    
	DevOledClearArea(ptOled, ucX, ucY, ucImageX, ucImageY);  
	
	for (Uint8 j = 0u; j < (ucImageY -1) / 8 + 1; j ++)		
	{		
		for (Uint8 i = 0u; i < ucImageX; i ++)
		{
			ptOled->aucGDDRAM[ucY / 8 + j][ucX + i] |= pucImageBuf[ucImageX * j + i] << (ucY % 8);			
			ptOled->aucGDDRAM[ucY / 8 + j + 1][ucX + i] |= pucImageBuf[ucImageX * j + i] >> (8 - ucY % 8);	
		}
	}
}

/*******************************************************************************
* 函数名称： DevOledDrawChar()
* 功能描述： ssd1315 oled 画字符函数
* 输入参数： ptOled:  oled结构体指针
            ucX: X偏移
            ucY: Y偏移
            ucChar: 要显示的字符
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 
Y(0-63)
 |
 |
 |______________ X(0-127)
(0,0)
*******************************************************************************/
void DevOledDrawChar(T_Oled *ptOled, Uint8 ucX, Uint8 ucY, Uint8 ucChar)
{
    DevOledDrawImage(ptOled, ucX, ucY, FONT_0608_WIDTH, FONT_0608_HEIGHT, OLED_FONT_0608[ucChar - ' ']);
}

/*******************************************************************************
* 函数名称： DevOledDrawString()
* 功能描述： ssd1315 oled 画字符串函数
* 输入参数： ptOled:  oled结构体指针
            ucX: X偏移
            ucY: Y偏移
            pucStr: 要显示的字符串
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 
Y(0-63)
 |
 |
 |______________ X(0-127)
(0,0)
*******************************************************************************/
void DevOledDrawString(T_Oled *ptOled, Uint8 ucX, Uint8 ucY, Uint8 *pucStr) 
{
	for (Uint8 i = 0u; pucStr[i] != '\0'; i++)		
	{
		DevOledDrawChar(ptOled, ucX + i * FONT_0608_WIDTH, ucY, pucStr[i]);
	}
}

static Uint32 DevOledPow(Uint8 ucNumber, Uint8 ucN)
{
	Uint32 uiResult = 1;	
	while(ucN --)			
	{
		uiResult *= ucNumber;		
	}
	return uiResult;
}

/*******************************************************************************
* 函数名称： DevOledDrawNum()
* 功能描述： ssd1315 oled 画数字函数
* 输入参数： ptOled:  oled结构体指针
            ucX: X偏移
            ucY: Y偏移
            iNumber: 要显示的数字
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 
Y(0-63)
 |
 |
 |______________ X(0-127)
(0,0)
*******************************************************************************/
void DevOledDrawNum(T_Oled *ptOled, Uint8 ucX, Uint8 ucY, Sint32 iNumber, Uint8 Length)
{
    if(iNumber < 0)    // 因为和有符号类型比较，0后不能加u，否则会被当成无符号数，导致条件永远为真
    {
        DevOledDrawChar(ptOled, ucX, ucY, '-');
        ucX += FONT_0608_WIDTH;
        iNumber = -iNumber;
    }
    else
    {
        DevOledDrawChar(ptOled, ucX, ucY, '+');
        ucX += FONT_0608_WIDTH;
    }

	for (Uint8 i = 0u; i < Length; i++)							
	{
		DevOledDrawChar(ptOled, ucX + i * FONT_0608_WIDTH, ucY, iNumber / DevOledPow(10, Length - i - 1) % 10 + '0');
	}
}

/*******************************************************************************
* 函数名称： DevSsd1315Init()
* 功能描述： ssd1315 oled 初始化
* 输入参数： ptOled  oled结构体指针
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 务必保证输入的参数不为NULL
*******************************************************************************/
void DevSsd1315Init(T_Oled *ptOled)
{
    ptOled->DcCtrl(0u);
    
    ptOled->ResetCtrl(0u);
    ptOled->ptI2c->DelayUs(20000);
    ptOled->ResetCtrl(1u);
    
    DevOledSendCmd(ptOled, 0xAE); //关闭显示
    
	DevOledSendCmd(ptOled, 0xD5); //设置时钟分频因子,震荡频率
	DevOledSendCmd(ptOled, 0x80); //[3:0],分频因子;[7:4],震荡频率，默认值0x80
    
	DevOledSendCmd(ptOled, 0xA8); //设置使能的驱动路数
	DevOledSendCmd(ptOled, 0x3F); //默认值0x3F(ptOled, 1/64) 
    
	DevOledSendCmd(ptOled, 0xD3); //设置显示偏移
	DevOledSendCmd(ptOled, 0x00); //默认为0x00

	DevOledSendCmd(ptOled, 0x40); //设置显示开始行
    
	DevOledSendCmd(ptOled, 0xA1); //段重定义设置,bit0:0,0->0;1,0->127;		
    
    DevOledSendCmd(ptOled, 0xDA); //设置COM硬件引脚配置
	DevOledSendCmd(ptOled, 0x12); //[5:4]配置
    
    DevOledSendCmd(ptOled, 0x81); //对比度设置

	DevOledSendCmd(ptOled, 0xEF); //1-0xFF(ptOled, 亮度设置,越大越亮)
    
    DevOledSendCmd(ptOled, 0xD9); //设置预充电周期
	DevOledSendCmd(ptOled, 0xF1); //[3:0],PHASE 1;[7:4],PHASE 2;
    
    DevOledSendCmd(ptOled, 0xDB); //设置VCOMH 电压倍率
	DevOledSendCmd(ptOled, 0x30); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
    
	DevOledSendCmd(ptOled, 0x20); //设置内存地址模式
	DevOledSendCmd(ptOled, 0x02); //[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
 
	DevOledSendCmd(ptOled, 0xC8); //设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数

	DevOledSendCmd(ptOled, 0xA4); //全局显示开启;bit0:1,开启;0,关闭;(ptOled, 白屏/黑屏)
	DevOledSendCmd(ptOled, 0xA6); //设置显示方式;bit0:1,反相显示;0,正常显示	

	DevOledSendCmd(ptOled, 0x8D); //电荷泵设置
	DevOledSendCmd(ptOled, 0x14); //开启
    
	DevOledSendCmd(ptOled, 0xAF); //开启显示	 
    
	DevOledClearAll(ptOled);
    
    DevOledRefresh(ptOled);
    
}

