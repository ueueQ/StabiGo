#ifndef _DRV_UART_H
#define _DRV_UART_H

#include "usart.h"
#include "BaseTypes.h"
#include "SimpleFifo.h"

/*******************************************************************************
* 宏定义 *
*******************************************************************************/
#define UART_TX_DMA_SIZE  256u  // Dma发送大小
#define UART_RX_DMA_SIZE  256u  // Dma接收大小

#define UART_TX_FIFO_SIZE  512u // FIFO发送大小
#define UART_RX_FIFO_SIZE  512u // FIFO接收大小

/*******************************************************************************
* 结束宏定义 *
*******************************************************************************/

/*******************************************************************************
* 全局类型 *
*******************************************************************************/
typedef struct Uart
{
    UART_HandleTypeDef *huart;  // 串口外设指针

    Uint8  aucTxDmaBuf[UART_TX_DMA_SIZE];  // 发送DMA缓冲区
    Uint16 uwTxDmaBufTotalSize;            // TxDmaBuf总大小
    volatile Bool bTxDmaBufSendCpltFlag;   // TxDmaBuf发送完成标志

    Uint8  aucRxDmaBuf[UART_RX_DMA_SIZE];  // 接收DMA缓冲区
    Uint16 uwRxDmaBufTotalSize;            // RxDmaBuf总大小
    Uint16 uwRxDmaBufLastPos;              // 已处理的RxDmaBuf位置

    T_Fifo tTxFifo;  // 发送FIFO
    Uint8  aucTxFifoBuf[UART_TX_FIFO_SIZE];  // 发送FIFO缓冲区
    Uint8  ucTxFifoWriteItem; // FIFO写入数据条
    Uint8  ucTxFifoReadItem;  // FIFO读取数据条

    T_Fifo tRxFifo;  // 接收FIFO
    Uint8  aucRxFifoBuf[UART_RX_FIFO_SIZE];  // 接收FIFO缓冲区
} T_Uart;

/*******************************************************************************
* 结束全局类型 *
*******************************************************************************/

/*******************************************************************************
* 全局函数原型 *
*******************************************************************************/

void DrvUartInit(T_Uart *ptUart, UART_HandleTypeDef *huart);
void DrvUartClearRx(T_Uart *ptUart);
void DrvUartClearTx(T_Uart *ptUart);
void DrvUartRxDmaHandler(T_Uart *ptUart, uint16_t uwRxDmaBufCurrPos);
Sint8 DrvUartRxFifoRead(T_Uart *ptUart, Uint8 *pucData, Uint16 uwLen);
Sint8 DrvUartRxFifoReadAll(T_Uart *ptUart, Uint8 *pucData);
Uint16 DrvUartTxDmaHandler(T_Uart *ptUart);
Sint8 DrvUartTxFifoWrite(T_Uart *ptUart, Uint8 *pucData, Uint16 uwLen);
/*******************************************************************************
* 结束全局函数原型 *
*******************************************************************************/

#endif

