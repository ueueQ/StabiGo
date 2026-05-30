#include "DrvI2cSw.h"


/*******************************************************************************
* 函数名称： DrvI2cSwStart()
* 功能描述： 启动信号
* 输入参数： ptI2c:  I2c初始化结构方法
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
*******************************************************************************/
void DrvI2cSwStart(T_I2cSw *ptI2c)
{
    ptI2c->SdaOut(1u);
    ptI2c->SclOut(1u);
    
    ptI2c->DelayUs(ptI2c->ucDelayUs);
    ptI2c->SdaOut(0u);
    ptI2c->DelayUs(ptI2c->ucDelayUs);
    ptI2c->SclOut(0u);
    ptI2c->DelayUs(ptI2c->ucDelayUs);
}

/*******************************************************************************
* 函数名称： DrvI2cSwStop()
* 功能描述： 停止信号
* 输入参数： ptI2c:  I2c初始化结构方法
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
*******************************************************************************/
void DrvI2cSwStop(T_I2cSw *ptI2c)
{
    ptI2c->SdaOut(0u);
    ptI2c->SclOut(0u);

    ptI2c->DelayUs(ptI2c->ucDelayUs);
    ptI2c->SclOut(1u);
    ptI2c->DelayUs(ptI2c->ucDelayUs);
    ptI2c->SdaOut(1u);
    ptI2c->DelayUs(ptI2c->ucDelayUs);
}

/*******************************************************************************
* 函数名称： DrvI2cSwAck()
* 功能描述： 应答信号
* 输入参数： ptI2c:  I2c初始化结构方法
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
*******************************************************************************/
void DrvI2cSwAck(T_I2cSw *ptI2c)    
{
    ptI2c->SdaOut(0u);
    ptI2c->DelayUs(ptI2c->ucDelayUs);
    
    ptI2c->SclOut(1u);
    ptI2c->DelayUs(ptI2c->ucDelayUs);
    ptI2c->SclOut(0u);
    ptI2c->DelayUs(ptI2c->ucDelayUs);
}

/*******************************************************************************
* 函数名称： DrvI2cSwAckNo()
* 功能描述： 非应答信号
* 输入参数： ptI2c:  I2c初始化结构方法
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
*******************************************************************************/
void DrvI2cSwAckNo(T_I2cSw *ptI2c)   
{
    ptI2c->SdaOut(1u);
    ptI2c->DelayUs(ptI2c->ucDelayUs);
    
    ptI2c->SclOut(1u);
    ptI2c->DelayUs(ptI2c->ucDelayUs);
    ptI2c->SclOut(0u);
    ptI2c->DelayUs(ptI2c->ucDelayUs);
}

/*******************************************************************************
* 函数名称： DrvI2cSwWaitAck()
* 功能描述： 等待应答信号
* 输入参数： ptI2c:  I2c初始化结构方法
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
*******************************************************************************/
Uint8 DrvI2cSwWaitAck(T_I2cSw *ptI2c)  
{
   Uint8 ucAck = 0u;
    
   ptI2c->SdaOut(1u);  
   ptI2c->DelayUs(ptI2c->ucDelayUs);
   ptI2c->SclOut(1u); 
   ptI2c->DelayUs(ptI2c->ucDelayUs);

    if (ptI2c->SdaRead())
    {
        ucAck = I2C_NACK;
    }
    else
    {
        ucAck = I2C_ACK;
    }
        
    ptI2c->SclOut(0u);
    ptI2c->DelayUs(ptI2c->ucDelayUs);

    return ucAck;
}

/*******************************************************************************
* 函数名称： DrvI2cSwSendByte()
* 功能描述： 发送一个字节
* 输入参数： ptI2c:  I2c初始化结构方法  
            ucValue： 字节
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
*******************************************************************************/
void DrvI2cSwSendByte(T_I2cSw *ptI2c, Uint8 ucValue)
{
    Uint8 ucTempValue = 0x80;
    Uint8 i = 0u;

    for (i = 0u; i < 8u; i++)
    {	
        ptI2c->SdaOut((ucValue&ucTempValue)>>7u);
        ucValue <<= 1u;
        ptI2c->DelayUs(ptI2c->ucDelayUs);

        ptI2c->SclOut(1u);    
        ptI2c->DelayUs(ptI2c->ucDelayUs);  
               
        ptI2c->SclOut(0u);   
        ptI2c->DelayUs(ptI2c->ucDelayUs); 
    }
}

/*******************************************************************************
* 函数名称： DrvI2cSwReadByte()
* 功能描述： 读取一个字节
* 输入参数： ptI2c:  I2c初始化结构方法  
            ucIsAck： 是否应答
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
*******************************************************************************/
Uint8 DrvI2cSwReadByte(T_I2cSw *ptI2c, Uint8 ucIsAck)
{
    Uint8 ucTempValue = 0u;
    Uint8 i = 0u;

    ptI2c->SdaOut(1u);

    for (i = 0u; i < 8u; i++)
    {

        ptI2c->SclOut(1u);   
        ptI2c->DelayUs(ptI2c->ucDelayUs);
        
        ucTempValue <<= 1u;
        if (ptI2c->SdaRead())
        {
            ucTempValue |= 0x01;
        }
        
        ptI2c->SclOut(0u);   
        ptI2c->DelayUs(ptI2c->ucDelayUs);
    }		

    if (ucIsAck)
    {
        DrvI2cSwAckNo(ptI2c);
    }
    else
    {
        DrvI2cSwAck(ptI2c);
    }

    return ucTempValue;
}

