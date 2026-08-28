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
#include "tiny_eds.h"
#include "DrvTimeStamp.h"
#include "TaskUart.h"
#include "TaskKey.h"
#include "TaskOled.h"

/*******************************************************************************
* Us延迟相关 *
*******************************************************************************/
#define TIM_BASE_US htim6

static Uint16 TimUsGetCounter(void)
{
	return __HAL_TIM_GET_COUNTER(&TIM_BASE_US);
}

/*******************************************************************************
* LED相关 *
*******************************************************************************/
static void LedCtrl(void)
{
    HAL_GPIO_TogglePin(LED_STATE_GPIO_Port, LED_STATE_Pin);
}

/*******************************************************************************
* TinyEDS相关 *
*******************************************************************************/
time_event_t gTimeEvent10ms[]   = {{TaskKeyUpdate, 1u}};
time_event_t gTimeEvent20ms[]   = {{TaskOledUpdate, 1u}};
time_event_t gTimeEvent1000ms[] = {{LedCtrl, 1u}};

notify_event_t gNotifyEvent[] = {{TaskDebugUartParseHandler, 1u}};

/*******************************************************************************
* 结束TinyEDS相关 *
*******************************************************************************/

static void BoardPinInit(void)
{

}

static void DriverInit(void)
{
    /* us延迟初始化 */
	HAL_TIM_Base_Start(&TIM_BASE_US);
    DrvUsDelayInit(TimUsGetCounter);

    /* Debug串口初始化 */
    DrvUartInit(&gtDebugUart, &huart1);
    HAL_UARTEx_ReceiveToIdle_DMA(gtDebugUart.huart, gtDebugUart.aucRxDmaBuf, gtDebugUart.uwRxDmaBufTotalSize);
    __HAL_DMA_DISABLE_IT(gtDebugUart.huart->hdmarx, DMA_IT_HT);

    /* 按键初始化 */
    DrvKeyInit(&gtKeyManager);

}

static void DeviceInit(void)
{
    /* OLED 初始化 */
    DevSsd1315Init(&gtOled);
}

static void ModuleInit(void)
{

}

static void AppInit(void)
{
    tiny_eds_init(DrvTspGet);
    REGISTER_MS_EVENT(PERIOD_10MS,   gTimeEvent10ms);
    REGISTER_MS_EVENT(PERIOD_20MS,   gTimeEvent20ms);
    REGISTER_MS_EVENT(PERIOD_1000MS, gTimeEvent1000ms);
    
    REGISTER_NOTIFY_EVENT(gNotifyEvent);
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

void System_Main_Loop(void)
{
    tiny_eds_run();
}


