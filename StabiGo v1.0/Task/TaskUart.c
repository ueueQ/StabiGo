#include "TaskUart.h"

#define UART_DEBUG_HANDLE huart1
T_Uart gtDebugUart = {0u};

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    /*
        注意：当使用DMA方式接收时，Size 参数表示当前DMA缓冲BUF已经填充的数据量，
        并不能将该 Size 作为本次需要处理的数据长度，因为之前的数据可能已经被处理过了。
    */
    HAL_UART_RxEventTypeTypeDef eventType = HAL_UARTEx_GetRxEventType(huart);
    Uint16 uwValidDataSize = 0u;

    if (huart->Instance == USART1)
    {
        if (eventType == HAL_UART_RXEVENT_IDLE) 
        {
            uwValidDataSize = Size;
        } else if (eventType == HAL_UART_RXEVENT_TC) 
        {
            uwValidDataSize = UART_RX_DMA_SIZE;
        } else if (eventType == HAL_UART_RXEVENT_HT) 
        {
            uwValidDataSize = UART_RX_DMA_SIZE >> 1;
        }

        DrvUartRxDmaHandler(&gtDebugUart, uwValidDataSize);
        notify_event_trigger(0u);
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    Uint16 uwValidSize = 0u;

    if (huart->Instance == USART1) {
        // 这里才是真正所有数据都已发送到线路上的时刻
        // 因为 UART_IT_TC 表示移位寄存器空了，数据全发出去了
        // 比 DMA_IT_TC 更“真实”地反映发送完成
        uwValidSize = DrvUartTxDmaHandler(&gtDebugUart);
        if(0u != uwValidSize) 
        {
            HAL_UART_Transmit_DMA(gtDebugUart.huart, gtDebugUart.aucTxDmaBuf, uwValidSize);
            __HAL_DMA_DISABLE_IT(gtDebugUart.huart->hdmatx, DMA_IT_HT);
        } 
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        HAL_UART_AbortReceive(gtDebugUart.huart);
        DrvUartClearRx(&gtDebugUart);
        HAL_UARTEx_ReceiveToIdle_DMA(gtDebugUart.huart, gtDebugUart.aucRxDmaBuf, gtDebugUart.uwRxDmaBufTotalSize);
        __HAL_DMA_DISABLE_IT(gtDebugUart.huart->hdmarx, DMA_IT_HT);
    }
}


void TaskDebugUartParseHandler(void)
{
    Uint8 rx_buffer[512] = {0u};
    Uint16 uwUsedNum = FiFoUsedNum(&gtDebugUart.tRxFifo);
    DrvUartRxFifoRead(&gtDebugUart, rx_buffer, uwUsedNum);
    DrvUartTxFifoWrite(&gtDebugUart, rx_buffer, uwUsedNum);
}
