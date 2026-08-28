#include "DrvKey.h"

/*******************************************************************************
* 函数名称： DrvKeyInit()
* 功能描述： 初始化按键管理器
* 输入参数： ptKeyManager:  按键管理器结构指针
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
*******************************************************************************/
void DrvKeyInit(T_KeyManager *ptKeyManager)
{
    if (ptKeyManager == NULL || ptKeyManager->ptKeys == NULL || ptKeyManager->ucKeyCount == 0)
    {
        return;
    }

    ptKeyManager->ucKeysPressed = 0u;
    ptKeyManager->event = KEY_EVENT_NONE;
}

/*******************************************************************************
* 函数名称： DrvKeyUpdate()
* 功能描述： 更新按键状态
* 输入参数： ptKeyManager:  按键管理器结构指针
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
*******************************************************************************/
void DrvKeyUpdate(T_KeyManager *ptKeyManager)
{
    T_Key *ptKey = NULL;
    Uint8 ucKeySate = 0u;
    Uint8 ucKeysReleasedCount = 0u;
    Uint8 ucKeysPressedCount = 0u;
    
    if (ptKeyManager == NULL)
    {
        return;
    }

    for (Uint8 i = 0; i < ptKeyManager->ucKeyCount; i++)
    {
        ptKey = &ptKeyManager->ptKeys[i];
        ucKeySate = ptKey->GetKeyState();
        if(ucKeySate != ptKey->ucCurrentLevel)
        {   
            ptKey->ucCurrentLevel = ucKeySate;
            ptKey->uiReverseFirstTime = ptKeyManager->GetTick();
        }
        else
        {
            if(ptKey->ucCurrentLevel == ptKey->ucPressLevel)
            {
                if(ptKeyManager->GetTick() - ptKey->uiReverseFirstTime >= ptKey->uwLongPressThreshold)
                {
                    ptKey->state = KEY_STATE_LONG_PRESS;
                }
                else if(ptKeyManager->GetTick() - ptKey->uiReverseFirstTime >= KEY_DEBOUNCE_TIME_MS)
                {
                    ptKey->state = KEY_STATE_PRESSED;
                }
            }
            else
            {
                ucKeysReleasedCount++;

                if(ptKey->state != KEY_STATE_IDLE)
                {
                    ucKeysPressedCount++;
                    ptKeyManager->ucKeysPressed |= (1u << i);
                }
                else
                {
                    ptKeyManager->ucKeysPressed &= ~(1u << i);
                }
            }
        }
    }

    /* 判定按键情况 */
    if((ucKeysReleasedCount == ptKeyManager->ucKeyCount)&&(ucKeysPressedCount > 0u))
    {
        ptKeyManager->event = ucKeysPressedCount == 1u? KEY_EVENT_ALONE : KEY_EVENT_GROUP;
        ucKeysPressedCount = 0u;
    }
    else
    {
        ptKeyManager->event = KEY_EVENT_NONE;
    }

    /* 按键处理 */
    switch (ptKeyManager->event)
    {
        case KEY_EVENT_ALONE:
                    for (Uint8 i = 0; i < ptKeyManager->ucKeyCount; i++)
                    {
                        ptKey = &ptKeyManager->ptKeys[i];
                        if(ptKey->state == KEY_STATE_PRESSED)
                        {
                            if(ptKey->on_click != NULL)
                            {
                                ptKey->on_click();
                            }
                        }
                        else if(ptKey->state == KEY_STATE_LONG_PRESS)
                        {
                            if(ptKey->on_long_press != NULL)
                            {
                                ptKey->on_long_press();
                            }
                        }

                        ptKey->state = KEY_STATE_IDLE;
                        ptKeyManager->event = KEY_EVENT_NONE;
                    }
            break;
        case KEY_EVENT_GROUP:
                    if(ptKeyManager->on_group != NULL)
                    {
                        ptKeyManager->on_group();
                    }

                    for (Uint8 i = 0; i < ptKeyManager->ucKeyCount; i++)
                    {
                        ptKey = &ptKeyManager->ptKeys[i];
                        ptKey->state = KEY_STATE_IDLE;
                    }

                    ptKeyManager->event = KEY_EVENT_NONE;
            break;
        default:
            break;
    }
}