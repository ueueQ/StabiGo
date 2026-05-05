#include "DrvI2cSw.h"


/*******************************************************************************
* 函数名称： Start()
* 功能描述： 启动信号
* 输入参数： ptI2c:  I2c初始化结构方法
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
*	
* 修改日期          版本号          修改人          修改内容
* ----------------------------------------------------------
* 2025/10/07        V0.01           cjs
*******************************************************************************/
static void Start(T_I2c *ptI2c)
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
* 函数名称： Stop()
* 功能描述： 终止信号
* 输入参数： ptI2c:  I2c初始化结构方法
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
*	
* 修改日期          版本号          修改人          修改内容
* ----------------------------------------------------------
* 2025/10/07        V0.01           cjs
*******************************************************************************/
static void Stop(T_I2c *ptI2c)
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
* 函数名称： Ack()
* 功能描述： 应答信号
* 输入参数： ptI2c:  I2c初始化结构方法
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 第九个时钟信号，sda拉低
*	
* 修改日期          版本号          修改人          修改内容
* ----------------------------------------------------------
* 2025/10/07        V0.01           cjs
*******************************************************************************/
static void Ack(T_I2c *ptI2c)     // 主机给从机回复的信号
{
    ptI2c->SdaOut(0u);
    ptI2c->DelayUs(ptI2c->ucDelayUs);
    
    ptI2c->SclOut(1u);
    ptI2c->DelayUs(ptI2c->ucDelayUs);
    ptI2c->SclOut(0u);
    ptI2c->DelayUs(ptI2c->ucDelayUs);
}

/*******************************************************************************
* 函数名称： AckNo()
* 功能描述： 非应答信号
* 输入参数： ptI2c:  I2c初始化结构方法
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 第九个时钟信号，sda拉高
*	
* 修改日期          版本号          修改人          修改内容
* ----------------------------------------------------------
* 2025/10/07        V0.01           cjs
*******************************************************************************/
static void AckNo(T_I2c *ptI2c)   // 主机给从机回复的信号
{
    ptI2c->SdaOut(1u);
    ptI2c->DelayUs(ptI2c->ucDelayUs);
    
    ptI2c->SclOut(1u);
    ptI2c->DelayUs(ptI2c->ucDelayUs);
    ptI2c->SclOut(0u);
    ptI2c->DelayUs(ptI2c->ucDelayUs);
}

/*******************************************************************************
* 函数名称： WaitAck()
* 功能描述： 等待从机应答信号
* 输入参数： ptI2c:  I2c初始化结构方法
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
*	
* 修改日期          版本号          修改人          修改内容
* ----------------------------------------------------------
* 2025/10/07        V0.01           cjs
*******************************************************************************/
static Uint8 WaitAck(T_I2c *ptI2c)  // 主机操作：主机发送完数据之后，检测从机是否有回应
{
   Uint8 ucAck = 0u;
    
   ptI2c->SdaOut(1u);  // 主机释放SDA，让从机去拉高或拉低，供主机来检测
   ptI2c->DelayUs(ptI2c->ucDelayUs);
   ptI2c->SclOut(1u);  // 拉高SCL，保证SDA线数据有效
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
* 函数名称： SendByte()
* 功能描述： 发送一个byte
* 输入参数： ptI2c:  I2c初始化结构方法
			ucValue：发送的值
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 内部无应答等待
*	
* 修改日期          版本号          修改人          修改内容
* ----------------------------------------------------------
* 2025/10/07        V0.01           cjs
*******************************************************************************/
static void SendByte(struct I2c *ptI2c, Uint8 ucValue)
{
    Uint8 ucTempValue = 0x80;
    Uint8 i = 0u;

    for (i = 0u; i < 8u; i++)
    {	
        ptI2c->SdaOut((ucValue&ucTempValue)>>7u);
        ucValue <<= 1u;
        ptI2c->DelayUs(ptI2c->ucDelayUs);

        ptI2c->SclOut(1u);    // 主机拉高，让从机读取
        ptI2c->DelayUs(ptI2c->ucDelayUs);  
               
        ptI2c->SclOut(0u);    // 主机拉低，钳住总线
        ptI2c->DelayUs(ptI2c->ucDelayUs); 
    }
}

/*******************************************************************************
* 函数名称： ReadByte()
* 功能描述： 接收一个byte
* 输入参数： ptI2c:  I2c初始化结构方法
						ucIsAck：是否应答
* 输出参数： 无
* 返 回 值： 读取到的一个byte
* 其它说明： 无
*	
* 修改日期          版本号          修改人          修改内容
* ----------------------------------------------------------
* 2025/10/07        V0.01           cjs
*******************************************************************************/
static Uint8 ReadByte(struct I2c *ptI2c, Uint8 ucIsAck)
{
    Uint8 ucTempValue = 0u;
    Uint8 i = 0u;

    ptI2c->SdaOut(1u);    // 注意：这里必须要拉高，主机释放SDA线权，等待从机发送数据。否则连续读操作无法使用

    for (i = 0u; i < 8u; i++)
    {

        ptI2c->SclOut(1u);   // 主机控制SCL拉高，稳住SDA。
        ptI2c->DelayUs(ptI2c->ucDelayUs);
        
        ucTempValue <<= 1u;
        if (ptI2c->SdaRead())
        {
            ucTempValue |= 0x01;
        }
        
        ptI2c->SclOut(0u);   // 主机控制SCL拉低，从机可以改变SDA。
        ptI2c->DelayUs(ptI2c->ucDelayUs);
    }		

    if (ucIsAck)
    {
        ptI2c->AckNo(ptI2c);
    }
    else
    {
        ptI2c->Ack(ptI2c);
    }

    return ucTempValue;
}

/*******************************************************************************
* 函数名称： I2cInit()
* 功能描述： I2c结构体初始化函数
* 输入参数： ptI2c:  I2c初始化结构方法
						
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 务必保证应该注册的都已经注册
*	
* 修改日期          版本号          修改人          修改内容
* ----------------------------------------------------------
* 2025/10/07        V0.01           cjs
*******************************************************************************/
void I2cInit(T_I2c *ptI2c)
{
    ptI2c->Start    = Start;
    ptI2c->Stop     = Stop;
    ptI2c->Ack      = Ack;
    ptI2c->AckNo    = AckNo;
    ptI2c->WaitAck  = WaitAck;
    ptI2c->SendByte = SendByte;
    ptI2c->ReadByte = ReadByte;
}
