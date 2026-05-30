#include "DrvUsDelay.h"

/*******************************************************************************
* 全局变量 *
*******************************************************************************/
static FuncDrvUsGetCounter DrvUsGetCounter = NULL;

/*******************************************************************************
* 结束全局变量 *
*******************************************************************************/

/*******************************************************************************
* 函数名称： DrvUsDelayInit()
* 功能描述： 注册16位的计数器返回值
* 输入参数： 16位的计数器返回值
* 输出参数： 无
* 返 回 值： 无
* 其它说明：无
*******************************************************************************/
void DrvUsDelayInit(FuncDrvUsGetCounter Func)
{
	if(Func != NULL)
	{
		DrvUsGetCounter = Func;
	}
}

/*******************************************************************************
* 函数名称： DrvBlockingDelayUs()
* 功能描述： us级阻塞式延迟函数
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明： ①最长延迟时间为65535us
            ②注意：由于函数调用以及其他语句执行等问题，精度不可能达到1us。
*******************************************************************************/
void DrvBlockingDelayUs(Uint16 uwDelayUs)
{
    Uint16 uwStartCounter = DrvUsGetCounter();
    Uint16 uwCurrCounter  = 0u;
    Uint16 uwGapCounter   = 0u;
        
    while(uwGapCounter < uwDelayUs)
    {
        uwCurrCounter = DrvUsGetCounter();
        
        if(uwCurrCounter >= uwStartCounter)
        {
            uwGapCounter = uwCurrCounter - uwStartCounter;
        }
        else
        {
            uwGapCounter = 0xFFFF - uwStartCounter + uwCurrCounter;
        }
    }
}
