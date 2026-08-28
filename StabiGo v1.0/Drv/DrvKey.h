#ifndef _DRV_KEY_H
#define _DRV_KEY_H

#include "BaseTypes.h"

/*******************************************************************************
* 宏定义 *
*******************************************************************************/
#define KEY_DEBOUNCE_TIME_MS  20u   // 按键消抖时间（ms）

// 按键电平定义
typedef enum {
    KEY_PRESS_LEVEL = 0,   // 按下为低电平
    KEY_RELEASE_LEVEL = 1  // 松开为高电平
} KeyLevel_t;

// 按键内部状态
typedef enum {
    KEY_STATE_IDLE = 0,    // 空闲
    KEY_STATE_PRESSED,     // 已按下
    KEY_STATE_LONG_PRESS   // 已确认长按
} KeyState_t;

// 按键事件类型
typedef enum {
    KEY_EVENT_NONE = 0,
    KEY_EVENT_ALONE,      // 单按键
    KEY_EVENT_GROUP       // 组合按键
} KeyEvent_t;
/*******************************************************************************
* 结束宏定义 *
*******************************************************************************/

/*******************************************************************************
* 全局类型 *
*******************************************************************************/
typedef struct Key
{
    Uint8 (*GetKeyState)(void);   // 获取按键电平状态的函数指针
    Uint8 ucPressLevel;           // 按时的电平值

    Uint32 uiReverseFirstTime;   // 按键电平首次反转时间戳
    Uint16 uwLongPressThreshold;  // 长按判定阈值（ms）
    
    // ---------- 状态变量 ----------
    KeyState_t state;        // 当前状态
    Uint8 ucCurrentLevel;    // 当前电平值
    Uint8 ucStableLevel;     // 稳态电平值

    // ---------- 回调函数 ----------
    void (*on_click)(void);        // 单击回调
    void (*on_long_press)(void);   // 长按回调

} T_Key;


typedef struct KeyManager
{
    T_Key *ptKeys;              // 按键数组
    Uint8 ucKeyCount;           // 按键数量

    Uint8 ucKeysPressed;        // 每个按键按下的状态 bit表示 最低位表示第一个按键，1表示按下，0表示松开
    KeyEvent_t event;           // 事件类型
    Uint32 (*GetTick)(void);    // 获取系统时间戳的函数指针（单位：ms）

    void (*on_group)(void);        // 组合按键事件回调
} T_KeyManager;

/*******************************************************************************
* 结束全局类型 *
*******************************************************************************/

/*******************************************************************************
* 全局函数原型 *
*******************************************************************************/

void DrvKeyInit(T_KeyManager *ptKeyManager);
void DrvKeyUpdate(T_KeyManager *ptKeyManager);
/*******************************************************************************
* 结束全局函数原型 *
*******************************************************************************/

#endif

