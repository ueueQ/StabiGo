#include "DrvUart.h"

/*******************************************************************************
 * 函数名称： DrvUartInit()
 * 功能描述： 初始化串口Drv函数
 * 输入参数： ptUart:     UART 实例指针
 *           huart:     UART 外设指针
 * 输出参数： 无
 * 返 回 值： 无
 * 其它说明： 初始化串口处理指针，内置Dma处理，Fifo缓冲
 ******************************************************************************/
void DrvUartInit(T_Uart *ptUart, UART_HandleTypeDef *huart)
{
    if (ptUart == NULL)
    {
        return;
    }

    ptUart->huart = huart;

    ptUart->uwRxDmaBufTotalSize = UART_RX_DMA_SIZE;
    ptUart->uwTxDmaBufTotalSize = UART_TX_DMA_SIZE;

    FiFoInit(&ptUart->tTxFifo, ptUart->aucTxFifoBuf, UART_TX_FIFO_SIZE);
    ptUart->bTxDmaBufSendCpltFlag = TRUE; 
    FiFoInit(&ptUart->tRxFifo, ptUart->aucRxFifoBuf, UART_RX_FIFO_SIZE);
    ptUart->uwRxDmaBufLastPos = 0u;
}

/*******************************************************************************
 * 函数名称： DrvUartClearRx()
 * 功能描述： 清空UART RX FIFO和DMA缓冲区
 * 输入参数： ptUart:     UART 实例指针
 * 输出参数： 无
 * 返 回 值： 无
 * 其它说明： 无
 ******************************************************************************/
void DrvUartClearRx(T_Uart *ptUart)
{
    if (ptUart == NULL)
    {
        return;
    }

    FiFoFlush(&ptUart->tRxFifo);
    ptUart->uwRxDmaBufLastPos = 0u;
}

/*******************************************************************************
 * 函数名称： DrvUartClearTx()
 * 功能描述： 清空UART TX FIFO和DMA缓冲区
 * 输入参数： ptUart:     UART 实例指针
 * 输出参数： 无
 * 返 回 值： 无
 * 其它说明： 无
 ******************************************************************************/
void DrvUartClearTx(T_Uart *ptUart)
{
    if (ptUart == NULL)
    {
        return;
    }

    FiFoFlush(&ptUart->tTxFifo);
}

/*******************************************************************************
 * 函数名称： DrvUartRxDmaHandler()
 * 功能描述： 放入HAL_UARTEx_RxEventCallback回调函数中处理，处理DMA接收数据，放入FIFO中
 * 输入参数： ptUart：    UART实例指针 
 *           uwRxDmaBufCurrPos:  当前DMA接收缓冲区位置
 * 输出参数： 无
 * 返 回 值： 无
 * 其它说明： 无
 ******************************************************************************/
void DrvUartRxDmaHandler(T_Uart *ptUart, uint16_t uwRxDmaBufCurrPos)
{
    if (ptUart == NULL || uwRxDmaBufCurrPos == 0)
    {
        return;
    }
    
    Uint16 uwValidDataSize = uwRxDmaBufCurrPos - ptUart->uwRxDmaBufLastPos;
    FiFoPuts(&ptUart->tRxFifo, &ptUart->aucRxDmaBuf[ptUart->uwRxDmaBufLastPos], uwValidDataSize);
    ptUart->uwRxDmaBufLastPos = uwRxDmaBufCurrPos == UART_RX_DMA_SIZE ? 0 : uwRxDmaBufCurrPos;  
}

/*******************************************************************************
 * 函数名称： DrvUartRxFifoRead()
 * 功能描述： 从UART接收FIFO中读取数据
 * 输入参数： ptUart：    UART实例指针
 *           pucData:   接收数据缓冲区指针
 *           uwLen:     要读取的数据长度
 * 输出参数： 无
 * 返 回 值： 无
 * 其它说明： 无
 ******************************************************************************/
Sint8 DrvUartRxFifoRead(T_Uart *ptUart, Uint8 *pucData, Uint16 uwLen)
{
    if (ptUart == NULL || pucData == NULL || uwLen == 0u)
    {
        return -1;
    }

    return FiFoGets(&ptUart->tRxFifo, pucData, uwLen);
}

/*******************************************************************************
 * 函数名称： DrvUartRxFifoReadAll()
 * 功能描述： 从UART接收FIFO中读取所有数据
 * 输入参数： ptUart：    UART实例指针
 *           pucData:   接收数据缓冲区指针
 * 输出参数： 无
 * 返 回 值： 无
 * 其它说明： 无
 ******************************************************************************/
Sint8 DrvUartRxFifoReadAll(T_Uart *ptUart, Uint8 *pucData)
{
    if (ptUart == NULL || pucData == NULL)
    {
        return -1;
    }

    return FiFoGets(&ptUart->tRxFifo, pucData, ptUart->tRxFifo.uwUsedNum);
}

/*******************************************************************************
 * 函数名称： DrvUartTxDmaHandler()
 * 功能描述： 放入HAL_UART_TxCpltCallback回调函数中处理，将FIFO的数据，输送DMA发送数据
 * 输入参数： ptUart：    UART实例指针 
 * 输出参数： 无
 * 返 回 值： 无
 * 其它说明： 无
 ******************************************************************************/
Uint16 DrvUartTxDmaHandler(T_Uart *ptUart)
{
    Sint8 cRetval = 0; //返回值
    Uint16 uwValidSize = 0u;

    if (ptUart == NULL)
    {
        return 0u;
    }

    if (ptUart->ucTxFifoReadItem == ptUart->ucTxFifoWriteItem)
    {
        ptUart->bTxDmaBufSendCpltFlag = TRUE;
        return 0u;
    }

    cRetval = FiFoGets(&ptUart->tTxFifo, (Uint8*)&uwValidSize, sizeof(uwValidSize)); // 先读取数据长度
    if (cRetval != 0)
    {
        return 0u;
    }

    cRetval = FiFoGets(&ptUart->tTxFifo, ptUart->aucTxDmaBuf, uwValidSize); // 再读取数据内容
    if (cRetval != 0)
    {
        return 0u;
    }

    ptUart->ucTxFifoReadItem++;

    return uwValidSize;
}

/*******************************************************************************
 * 函数名称： DrvUartTxFifoWrite()
 * 功能描述： 向UART发送FIFO中写入数据
 * 输入参数： ptUart：    UART实例指针
 *           pucData:   发送数据缓冲区指针
 *           uwLen:     要写入的数据长度
 * 输出参数： 无
 * 返 回 值： 无
 * 其它说明： 无
 ******************************************************************************/
Sint8 DrvUartTxFifoWrite(T_Uart *ptUart, Uint8 *pucData, Uint16 uwLen)
{
    Sint8 cRetval = 0; //返回值

    if (ptUart == NULL || pucData == NULL || uwLen == 0u)
    {
        return -1;
    }

    if (ptUart->bTxDmaBufSendCpltFlag)
    {
        memcpy(ptUart->aucTxDmaBuf, pucData, uwLen);

        HAL_UART_Transmit_DMA(ptUart->huart, ptUart->aucTxDmaBuf, uwLen);
        __HAL_DMA_DISABLE_IT(ptUart->huart->hdmatx, DMA_IT_HT);
        ptUart->bTxDmaBufSendCpltFlag = FALSE;
    }
    else
    {
        cRetval = FiFoPuts(&ptUart->tTxFifo, (Uint8*)&uwLen, sizeof(uwLen)); // 先写入数据长度
        if (cRetval != 0)
        {
            return -1;
        }

        cRetval = FiFoPuts(&ptUart->tTxFifo, pucData, uwLen); // 再写入数据内容
        if (cRetval != 0)
        {
            FiFoDiscard(&ptUart->tTxFifo, sizeof(uwLen)); // 如果写入失败，丢弃长度信息
            return -1;
        }

        ptUart->ucTxFifoWriteItem++;

        if(ptUart->bTxDmaBufSendCpltFlag)
        {
            FiFoFlush(&ptUart->tTxFifo); // 清空FIFO，防止重复发送
            ptUart->ucTxFifoWriteItem--;

            memcpy(ptUart->aucTxDmaBuf, pucData, uwLen);

            HAL_UART_Transmit_DMA(ptUart->huart, ptUart->aucTxDmaBuf, uwLen);
            __HAL_DMA_DISABLE_IT(ptUart->huart->hdmatx, DMA_IT_HT);
            ptUart->bTxDmaBufSendCpltFlag = FALSE;
        }
    }

    return 0u;
}

