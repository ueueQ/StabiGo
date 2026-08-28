#ifndef __SIMPLEFIFO_H__
#define __SIMPLEFIFO_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "BaseTypes.h"


typedef struct Fifo
{
    Uint8  *pucStartAddr;  // FIFO 内存池起始地址
    Uint8  *pucEndAddr;    // FIFO 内存池结束地址
    volatile Uint16  uwFreeNum;     // FIFO 剩余容量
    volatile Uint16  uwUsedNum;     // FIFO 中已使用的元素数量
    Uint16 uwTotalNum;     // FIFO 总容量
    Uint8  *pucReadIndex;  // FIFO 数据读索引
    Uint8  *pucWriteIndex; // FIFO 数据写索引
    Uint32 uiLostCount;    // FIFO 数据丢失次数累计
} T_Fifo;


Sint8  FiFoInit(T_Fifo *ptFifo, Uint8 *pucBaseBuf, Uint16 uwLen);
Sint8  FiFoPuts(T_Fifo *ptFifo, Uint8 *pucSource, Uint16 uwLen);
Sint8  FiFoGets(T_Fifo *ptFifo, Uint8 *pucDest, Uint16 uwLen);
Sint8  FiFoPreGets(T_Fifo *ptFifo, Uint8 *pucDest, Uint16 uwLen);
Sint8  FiFoIsEmpty(T_Fifo *ptFifo);
Sint8  FiFoIsFull(T_Fifo *ptFifo);
Sint16 FiFoUsedNum(T_Fifo *ptFifo);
Sint16 FiFoFreeNum(T_Fifo *ptFifo);
void   FiFoFlush(T_Fifo *ptFifo);
Sint16 FiFoDiscard(T_Fifo *ptFifo, Uint16 uwLen);


#endif // __SIMPLEFIFO_H__
