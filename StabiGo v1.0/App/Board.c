#include "main.h"
#include "Board.h"
#include "BaseTypes.h"
/*******************************************************************************
* HAL库头文件 *
*******************************************************************************/
#include "stm32f1xx_hal.h"
#include "tim.h"
/*******************************************************************************
* 用户头文件 *
*******************************************************************************/
#include "DrvUsDelay.h"
#include "DrvI2cSw.h"
#include "DevSsd1315.h"


/*******************************************************************************
* Us延迟相关 *
*******************************************************************************/
#define TIM_BASE_US htim6

static Uint16 TimUsGetCounter(void)
{
	return __HAL_TIM_GET_COUNTER(&TIM_BASE_US);
}

/*******************************************************************************
* Us延迟相关 *
*******************************************************************************/


/*******************************************************************************
* OLED相关 *
*******************************************************************************/
T_I2cSw  gtOledI2c = {0u};
T_Oled   gtOled  = {0u};

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
* LED相关 *
*******************************************************************************/




static void BoardPinInit(void)
{

}

static void DriverInit(void)
{
    /* us延迟初始化 */
	HAL_TIM_Base_Start(&TIM_BASE_US);
    DrvUsDelayInit(TimUsGetCounter);

    /* OLED I2C初始化 */
    gtOledI2c.DelayUs = DrvBlockingDelayUs;
    gtOledI2c.SclOut = OledSclOut;
    gtOledI2c.SdaOut = OledSdaOut;
    gtOledI2c.SdaRead = OledSdaRead;
}

static void DeviceInit(void)
{
    /* OLED 初始化 */
    gtOled.ucAddr = OLED_ADDR;
    gtOled.ptI2c = &gtOledI2c;
    gtOled.ResetCtrl = OledResetCtrl;
    gtOled.DcCtrl = OledDcCtrl;
    DevSsd1315Init(&gtOled);
  
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
    DevOledDrawPoint(&gtOled, 0u, 0u);
	HAL_GPIO_WritePin(LED_STATE_GPIO_Port, LED_STATE_Pin, GPIO_PIN_RESET);
	HAL_Delay(1000);
    DevOledDrawPoint(&gtOled, 127u, 63u);
	HAL_GPIO_WritePin(LED_STATE_GPIO_Port, LED_STATE_Pin, GPIO_PIN_SET);
    DevOledRefresh(&gtOled);
}