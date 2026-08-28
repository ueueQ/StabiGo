#include "SimpleFifo.h"

/*******************************************************************************
 * 函数名称： FiFoInit()
 * 功能描述： 初始化静态 FIFO 结构体（单字节模式）
 * 输入参数： ptFifo:     FIFO 实例指针
 *           pucBaseBuf: 预分配内存的基地址，例如数组
 *           uwLen:      FIFO 元素的数量
 * 输出参数： 无
 * 返 回 值： 0: 初始化成功  -1: 初始化失败
 * 其它说明： 无
 ******************************************************************************/
Sint8 FiFoInit(T_Fifo *ptFifo, Uint8 *pucBaseBuf, Uint16 uwLen)
{
    if (NULL == ptFifo || NULL == pucBaseBuf || 0 == uwLen)
    {
        return -1;
    }

    ptFifo->pucStartAddr = pucBaseBuf;
    ptFifo->pucEndAddr = pucBaseBuf + uwLen - 1;
    ptFifo->uwFreeNum = uwLen;
    ptFifo->uwUsedNum = 0;
    ptFifo->uwTotalNum = uwLen;
    ptFifo->pucReadIndex = pucBaseBuf;
    ptFifo->pucWriteIndex = pucBaseBuf;
    ptFifo->uiLostCount = 0;

    return (0);
}


/*******************************************************************************
 * 函数名称： FiFoPuts()
 * 功能描述： 向 FIFO 中放入多个元素，忽略中断保护
 * 输入参数： ptFifo:    FIFO 指针
 *           pucSource: 要放入的数据源
 *           uwLen:     要放入的元素数量
 * 输出参数： 无
 * 返 回 值： 0: 成功  -1: 失败  -2: 失败（数据不足）
 * 其它说明： 无
 ******************************************************************************/
Sint8 FiFoPuts(T_Fifo *ptFifo, Uint8 *pucSource, Uint16 uwLen)
{
    Uint16 uwLenToEnd;
    Uint16 uwLenFromStart;

    if (NULL == ptFifo || NULL == pucSource || 0 == uwLen)
    {
        return -1;
    }

    if (uwLen > ptFifo->uwFreeNum)
    {
        ptFifo->uiLostCount++;
        return -2;
    }

    if (ptFifo->pucWriteIndex > ptFifo->pucEndAddr)
    {
        ptFifo->pucWriteIndex = ptFifo->pucStartAddr;
    }

    uwLenToEnd = ptFifo->pucEndAddr - ptFifo->pucWriteIndex + 1;

    if (uwLenToEnd >= uwLen)
    {
        uwLenToEnd = uwLen;
        memcpy(ptFifo->pucWriteIndex, pucSource, uwLenToEnd);
        ptFifo->pucWriteIndex += uwLenToEnd;
    }
    else
    {
        uwLenFromStart = uwLen - uwLenToEnd;
        memcpy(ptFifo->pucWriteIndex, pucSource, uwLenToEnd);
        memcpy(ptFifo->pucStartAddr, pucSource + uwLenToEnd, uwLenFromStart);
        ptFifo->pucWriteIndex = ptFifo->pucStartAddr + uwLenFromStart;
    }

    ptFifo->uwFreeNum -= uwLen;
    ptFifo->uwUsedNum += uwLen;

    return 0;
}

/*******************************************************************************
 * 函数名称： FiFoGets()
 * 功能描述： 从 FIFO 中获取多个元素（单字节模式，无保护）
 * 输入参数： ptFifo:  FIFO 指针
 *           pucDest: 目标缓冲区
 *           uwLen:   要读取的元素数量
 * 输出参数： 无
 * 返 回 值： 0: 成功  -1: 失败  -2: 失败（数据不足）
 * 其它说明： 无
 ******************************************************************************/
Sint8 FiFoGets(T_Fifo *ptFifo, Uint8 *pucDest, Uint16 uwLen)
{
    Uint16 uwLenToEnd;
    Uint16 uwLenFromStart;

    if (NULL == ptFifo || NULL == pucDest || 0 == uwLen)
    {
        return -1;
    }

    if(uwLen > ptFifo->uwUsedNum)
    {
        return -2;
    }

    if (ptFifo->pucReadIndex > ptFifo->pucEndAddr)
    {
        ptFifo->pucReadIndex = ptFifo->pucStartAddr;
    }

    uwLenToEnd = ptFifo->pucEndAddr - ptFifo->pucReadIndex + 1;

    if (uwLenToEnd >= uwLen)
    {
        uwLenToEnd = uwLen;
        memcpy(pucDest, ptFifo->pucReadIndex, uwLenToEnd);
        ptFifo->pucReadIndex += uwLenToEnd;
    }
    else
    {
        uwLenFromStart = uwLen - uwLenToEnd;
        memcpy(pucDest, ptFifo->pucReadIndex, uwLenToEnd);
        memcpy(pucDest + uwLenToEnd, ptFifo->pucStartAddr, uwLenFromStart);
        ptFifo->pucReadIndex = ptFifo->pucStartAddr + uwLenFromStart;
    }

    ptFifo->uwFreeNum += uwLen;
    ptFifo->uwUsedNum -= uwLen;

    return 0;
}

/*******************************************************************************
 * 函数名称： FiFoPreGets()
 * 功能描述： 预读取 FIFO 中的多个元素（单字节模式，不消费数据）
 *           将数据拷贝到目标缓冲区，但不移动读指针，不修改 used/free 数量
 * 输入参数： ptFifo:  FIFO 指针
 *           pucDest: 目标缓冲区
 *           uwLen:   要预读的元素数量
 * 输出参数： 无
 * 返 回 值： 0: 成功  -1: 失败（参数非法）  -2: 失败（数据不足）
 * 其它说明： 与 FiFoGets 参数完全对称，区别仅在于不消费数据
 ******************************************************************************/
Sint8 FiFoPreGets(T_Fifo *ptFifo, Uint8 *pucDest, Uint16 uwLen)
{
    Uint16  uwLenToEnd;
    Uint16  uwLenFromStart;
    Uint8  *pucTmpRead;

    if (NULL == ptFifo || NULL == pucDest || 0 == uwLen)
    {
        return -1;
    }

    if (uwLen > ptFifo->uwUsedNum)
    {
        return -2;
    }

    /* 使用局部指针承接读索引归一化，避免修改 FIFO 状态（预读契约） */
    pucTmpRead = ptFifo->pucReadIndex;
    if (pucTmpRead > ptFifo->pucEndAddr)
    {
        pucTmpRead = ptFifo->pucStartAddr;
    }

    uwLenToEnd = ptFifo->pucEndAddr - pucTmpRead + 1;

    if (uwLenToEnd >= uwLen)
    {
        memcpy(pucDest, pucTmpRead, uwLen);
    }
    else
    {
        uwLenFromStart = uwLen - uwLenToEnd;
        memcpy(pucDest, pucTmpRead, uwLenToEnd);
        memcpy(pucDest + uwLenToEnd, ptFifo->pucStartAddr, uwLenFromStart);
    }

    return 0;
}

/*******************************************************************************
 * 函数名称： FiFoIsEmpty()
 * 功能描述： 判断 FIFO 是否为空（单字节模式）
 * 输入参数： ptFifo: FIFO 指针
 * 输出参数： 无
 * 返 回 值： 非零值(true): 为空  零值(false): 不为空
 * 其它说明： 无
 ******************************************************************************/
Sint8 FiFoIsEmpty(T_Fifo *ptFifo)
{
    if (NULL == ptFifo)
    {
        return -1;
    }
    return (ptFifo->uwUsedNum ? 0 : 1);
}

/*******************************************************************************
 * 函数名称： FiFoIsFull()
 * 功能描述： 判断 FIFO 是否已满（单字节模式）
 * 输入参数： ptFifo: FIFO 指针
 * 输出参数： 无
 * 返 回 值： 非零值(true): 已满  零值(false): 未满
 * 其它说明： 无
 ******************************************************************************/
Sint8 FiFoIsFull(T_Fifo *ptFifo)
{
    if (NULL == ptFifo)
    {
        return -1;
    }
    return (ptFifo->uwFreeNum ? 0 : 1);
}

/*******************************************************************************
 * 函数名称： FiFoUsedNum()
 * 功能描述： 获取 FIFO 中已使用的元素数量（单字节模式）
 * 输入参数： ptFifo: FIFO 指针
 * 输出参数： 无
 * 返 回 值： FIFO 中的元素数量
 * 其它说明： 无
 ******************************************************************************/
Sint16 FiFoUsedNum(T_Fifo *ptFifo)
{
    if (NULL == ptFifo)
    {
        return -1;
    }
    return ptFifo->uwUsedNum;
}

/*******************************************************************************
 * 函数名称： FiFoFreeNum()
 * 功能描述： 获取 FIFO 中空闲的元素数量（单字节模式）
 * 输入参数： ptFifo: FIFO 指针
 * 输出参数： 无
 * 返 回 值： FIFO 中的空闲元素数量
 * 其它说明： 无
 ******************************************************************************/
Sint16 FiFoFreeNum(T_Fifo *ptFifo)
{
    if (NULL == ptFifo)
    {
        return -1;
    }
    return ptFifo->uwFreeNum;
}

/*******************************************************************************
 * 函数名称： FiFoFlush()
 * 功能描述： 清空 FIFO 的内容
 * 输入参数： ptFifo: FIFO 指针
 * 输出参数： 无
 * 返 回 值： 无
 * 其它说明： 无
 ******************************************************************************/
void FiFoFlush(T_Fifo *ptFifo)
{
    if (NULL == ptFifo)
    {
        return;
    }
    ptFifo->uwFreeNum = ptFifo->pucEndAddr - ptFifo->pucStartAddr + 1;
    ptFifo->uwUsedNum = 0;
    ptFifo->pucReadIndex = ptFifo->pucStartAddr;
    ptFifo->pucWriteIndex = ptFifo->pucStartAddr;
}

/*******************************************************************************
 * 函数名称： FiFoDiscard()
 * 功能描述： 丢弃 FIFO 中指定长度的数据（单字节模式）
 *           直接移动读指针，跳过队头的 uwLen 个字节，不拷贝数据
 * 输入参数： ptFifo: FIFO 指针
 *           uwLen:  要丢弃的字节数
 * 输出参数： 无
 * 返 回 值： 实际丢弃的字节数
 * 其它说明： 无
 ******************************************************************************/
Sint16 FiFoDiscard(T_Fifo *ptFifo, Uint16 uwLen)
{
    Uint8 *pucTmpIndex;
    if (NULL == ptFifo)
    {
        return -1;
    }

    if (uwLen > ptFifo->uwUsedNum)
    {
        uwLen = ptFifo->uwUsedNum;
    }

    pucTmpIndex = uwLen + ptFifo->pucReadIndex;
    if (pucTmpIndex > ptFifo->pucEndAddr)
    {
        pucTmpIndex = pucTmpIndex - ptFifo->pucEndAddr + ptFifo->pucStartAddr - 1;
    }
    ptFifo->pucReadIndex = pucTmpIndex;
    ptFifo->uwFreeNum += uwLen;
    ptFifo->uwUsedNum -= uwLen;

    return uwLen;
}
