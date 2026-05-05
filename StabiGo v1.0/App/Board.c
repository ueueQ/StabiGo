#include "main.h"
#include "Board.h"
#include "BaseTypes.h"
/*******************************************************************************
* Hal库头文件相关*
*******************************************************************************/
#include "stm32f1xx_hal.h"
#include "tim.h"
/*******************************************************************************
* Drv层头文件相关*
*******************************************************************************/
#include "DrvUsDelay.h"
#include "DrvI2cSw.h"



/*******************************************************************************
* Us延迟相关*
*******************************************************************************/
#define TIM_BASE_US htim6

static Uint16 TimUsGetCounter(void)
{
	return __HAL_TIM_GET_COUNTER(&TIM_BASE_US);
}

/*******************************************************************************
* Us延迟相关*
*******************************************************************************/


/*******************************************************************************
* Oled屏幕相关*
*******************************************************************************/
//T_I2c  gtOledI2c;
//T_Oled gtOled;

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
* Oled屏幕相关*
*******************************************************************************/

/*******************************************************************************
* LED相关*
*******************************************************************************/



/*******************************************************************************
* LED相关*
*******************************************************************************/
 	
static void BoardPinInit(void)
{

}

static void DriverInit(void)
{
	DrvUsDelayInit(TimUsGetCounter);

}

static void DeviceInit(void)
{

}

static void ModuleInit(void)
{
}

static void AppInit(void)
{
}


void BoardInit(void)
{
    __disable_irq();
    BoardPinInit();
    DriverInit();
    DeviceInit();
    ModuleInit();
    AppInit();
    __enable_irq();
}

void loop(void)
{
	HAL_Delay(1000);
	HAL_GPIO_WritePin(LED_STATE_GPIO_Port, LED_STATE_Pin, GPIO_PIN_RESET);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(LED_STATE_GPIO_Port, LED_STATE_Pin, GPIO_PIN_SET);
}