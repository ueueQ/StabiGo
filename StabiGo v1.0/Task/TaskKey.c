#include "TaskKey.h"


Uint8 value1 = 0u;
Uint8 value2 = 0u;

/*******************************************************************************
* KEYA *
*******************************************************************************/
static Uint8 GetKeyAState(void)
{
    return (Uint8)HAL_GPIO_ReadPin(KEY_A_GPIO_Port, KEY_A_Pin);
}

static void KeyAOneClick(void)
{
    value1++;
}

static void KeyALongPress(void)
{
    value2++;
}

/*******************************************************************************
* KEYB *
*******************************************************************************/
static Uint8 GetKeyBState(void)
{
    return (Uint8)HAL_GPIO_ReadPin(KEY_B_GPIO_Port, KEY_B_Pin);
}

static void KeyBOneClick(void)
{
    value1--;
}

static void KeyBLongPress(void)
{
    value2--;
}

/*******************************************************************************
 * 函数名称： KeyGroupEvent()
 * 功能描述： 组合按键事件回调函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 其它说明： 无
 ******************************************************************************/
static void KeyGroupEvent(void)
{
    value1 = 0u;
    value2 = 0u;
}

/*******************************************************************************
 * 函数名称： TaskKeyUpdate()
 * 功能描述： 按键任务更新函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 其它说明： 无
 ******************************************************************************/
void TaskKeyUpdate(void)
{
    DrvKeyUpdate(&gtKeyManager);
}

/*******************************************************************************
* 全局变量 *
*******************************************************************************/
static T_Key gtKeys[2] = {
    {GetKeyAState, KEY_PRESS_LEVEL, 0u, 1000u, KEY_STATE_IDLE, KEY_RELEASE_LEVEL, KEY_RELEASE_LEVEL, KeyAOneClick, KeyALongPress},
    {GetKeyBState, KEY_PRESS_LEVEL, 0u, 1000u, KEY_STATE_IDLE, KEY_RELEASE_LEVEL, KEY_RELEASE_LEVEL, KeyBOneClick, KeyBLongPress}
};

T_KeyManager gtKeyManager = {gtKeys, 2u, 0u, KEY_EVENT_NONE, HAL_GetTick, KeyGroupEvent};

/*******************************************************************************
* 结束全局变量 *
*******************************************************************************/
