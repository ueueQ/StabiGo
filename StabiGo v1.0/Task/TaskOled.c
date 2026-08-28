#include "TaskOled.h"


static void OledSclOut(Uint8 ucValue)
{
    HAL_GPIO_WritePin(OLED_IIC_SCL_GPIO_Port, OLED_IIC_SCL_Pin, ucValue? GPIO_PIN_SET:GPIO_PIN_RESET);
}

static void OledSdaOut(Uint8 ucValue)
{
    HAL_GPIO_WritePin(OLED_IIC_SDA_GPIO_Port, OLED_IIC_SDA_Pin, ucValue? GPIO_PIN_SET:GPIO_PIN_RESET);
}

static Uint8 OledSdaRead(void)
{
    return (Uint8)HAL_GPIO_ReadPin(OLED_IIC_SDA_GPIO_Port, OLED_IIC_SDA_Pin);
}

static void OledResetCtrl(Uint8 ucValue)
{
    HAL_GPIO_WritePin(OLED_IIC_RES_GPIO_Port, OLED_IIC_RES_Pin, ucValue? GPIO_PIN_SET:GPIO_PIN_RESET);
}

static void OledDcCtrl(Uint8 ucValue)
{
    HAL_GPIO_WritePin(OLED_IIC_DC_GPIO_Port, OLED_IIC_DC_Pin, ucValue? GPIO_PIN_SET:GPIO_PIN_RESET);
}

/*******************************************************************************
 * 函数名称： TaskOledUpdate()
 * 功能描述： OLED任务更新函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 其它说明： 无
 ******************************************************************************/
void TaskOledUpdate(void)
{
    static Uint8 ucCount = 0u;
    ucCount++;
    DevOledDrawNum(&gtOled, 3u, 10u, ucCount, 5);
    DevOledDrawNum(&gtOled, 3u, 20u, -12345, 5);
    DevOledDrawString(&gtOled, 15u, 0u, (Uint8 *)"Hello StabiGo!");
    DevOledRefresh(&gtOled);
}

/*******************************************************************************
* 全局变量 *
*******************************************************************************/

T_I2cSw  gtOledI2c = {.DelayUs = DrvBlockingDelayUs, 
                      .SclOut  = OledSclOut, 
                      .SdaOut  = OledSdaOut, 
                      .SdaRead = OledSdaRead, 
                      .ucDelayUs = 1u};

T_Oled gtOled = {.aucGDDRAM = {0u}, 
                 .ucAddr    = OLED_ADDR, 
                 .ptI2c     = &gtOledI2c, 
                 .ResetCtrl = OledResetCtrl, 
                 .DcCtrl    = OledDcCtrl};

/*******************************************************************************
* 结束全局变量 *
*******************************************************************************/

