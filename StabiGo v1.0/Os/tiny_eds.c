#include "tiny_eds.h"
#include <string.h>

/*******************************************************************************
 * 静态TinyEDS实例
 *******************************************************************************/
static tiny_eds_t g_eds;

/*******************************************************************************
 * 内部函数声明
 *******************************************************************************/
static int8_t find_group_by_period(time_period_t period);
static void process_notify_round(void);
static uint8_t process_one_time_event(void);
static void check_time_events(void);

/*******************************************************************************
 * TinyEDS初始化
 *******************************************************************************/
void tiny_eds_init(uint64_t (*get_tick)(void))
{
    if (get_tick == NULL)
    {
        return;
    }

    memset(&g_eds, 0, sizeof(tiny_eds_t));

    /* 初始化7个周期组，按period从小到大排列 */
    g_eds.groups[0].period = PERIOD_10MS;
    g_eds.groups[1].period = PERIOD_20MS;
    g_eds.groups[2].period = PERIOD_50MS;
    g_eds.groups[3].period = PERIOD_100MS;
    g_eds.groups[4].period = PERIOD_200MS;
    g_eds.groups[5].period = PERIOD_500MS;
    g_eds.groups[6].period = PERIOD_1000MS;

    g_eds.get_tick = get_tick;

    /* 初始化各组的last_trigger为当前tick */
    uint64_t now = get_tick();
    for (uint8_t i = 0; i < PERIOD_GROUP_MAX; i++)
    {
        g_eds.groups[i].last_trigger = now;
    }

    g_eds.is_running = 0;
}

/*******************************************************************************
 * 注册时间事件
 * 指定周期和事件数组，内部自动归入对应周期组
 * period: 触发周期，仅允许time_period_t枚举值
 * evts: 时间事件数组
 * count: 数组长度
 * 返回值：0成功，-1失败
 *******************************************************************************/
int8_t register_time_event(time_period_t period, time_event_t *evts, uint8_t count)
{
    if (evts == NULL || count == 0 || count > TIME_EVENT_MAX)
    {
        return -1;
    }

    /* 检查回调函数有效性 */
    for (uint8_t i = 0; i < count; i++)
    {
        if (evts[i].callback == NULL)
        {
            return -1;
        }
    }

    /* 查找对应周期组 */
    int8_t grp_idx = find_group_by_period(period);
    if (grp_idx < 0)
    {
        return -1;  /* 周期值不合法 */
    }

    period_group_t *grp = &g_eds.groups[grp_idx];
    if (grp->events != NULL)
    {
        return -1;  /* 该组已注册，不可重复注册 */
    }

    /* 直接指向用户数组 */
    grp->events = evts;
    grp->event_count = count;

    return 0;
}

/*******************************************************************************
 * 注册通知事件
 * 传入事件数组，数组下标即优先级(bit0最高)
 * evts: 通知事件数组，第一个元素对应bit0，优先级最高
 * count: 数组长度，最大16
 * 返回值：0成功，-1失败
 *******************************************************************************/
int8_t register_notify_event(notify_event_t *evts, uint8_t count)
{
    if (evts == NULL || count == 0 || count > NOTIFY_EVENT_MAX)
    {
        return -1;
    }

    /* 检查回调函数有效性 */
    for (uint8_t i = 0; i < count; i++)
    {
        if (evts[i].callback == NULL)
        {
            return -1;
        }
    }

    if (g_eds.notify_events != NULL)
    {
        return -1;  /* 已注册，不可重复注册 */
    }

    g_eds.notify_events = evts;
    g_eds.notify_event_count = count;

    return 0;
}

/*******************************************************************************
 * 触发通知事件（中断中调用）
 * 将对应bit置1，表示该事件待执行
 *******************************************************************************/
void notify_event_trigger(uint8_t event_id)
{
    if (event_id < g_eds.notify_event_count)
    {
        g_eds.notify_pending |= (1u << event_id);
    }
}

/*******************************************************************************
 * TinyEDS运行
 *
 * 调度规则：
 * ① 通知事件按轮次执行：每轮从bit0扫描到bit15，执行所有pending的通知
 *    执行bit3期间若bit1又置1，不会立即执行bit1，而是先完成本轮bit4~bit15
 * ② 一轮通知扫描完毕后，检查时间事件是否到期
 * ③ 若有时间任务到期，执行一个时间任务，然后回到①重新检查通知
 * ④ 若无时间任务，直接回到①开始新一轮通知检查
 *******************************************************************************/
void tiny_eds_run(void)
{
    g_eds.is_running = 1;

    while (g_eds.is_running)
    {
        /* 1. 执行一轮通知扫描（bit0~bit15） */
        process_notify_round();

        /* 2. 检查时间事件是否到期 */
        check_time_events();

        /* 3. 尝试执行一个时间任务 */
        if (process_one_time_event())
        {
            /* 执行了一个时间任务，回到步骤1检查通知 */
            continue;
        }

        /* 4. 无时间任务，回到步骤1继续新一轮通知检查 */
    }
}

/*******************************************************************************
 * TinyEDS停止
 *******************************************************************************/
void tiny_eds_stop(void)
{
    g_eds.is_running = 0;
}

/*******************************************************************************
 * 内部函数实现
 *******************************************************************************/

/* 查找周期组下标 */
static int8_t find_group_by_period(time_period_t period)
{
    for (uint8_t i = 0; i < PERIOD_GROUP_MAX; i++)
    {
        if (g_eds.groups[i].period == period)
        {
            return (int8_t)i;
        }
    }
    return -1;
}

/*******************************************************************************
 * 检查时间事件是否到期
 * 每组独立判定：now - last_trigger >= period 则到期
 * 到期时置pending，记录last_trigger为当前时间
 * pending标志避免ready期间丢失新周期触发 
 *******************************************************************************/
static void check_time_events(void)
{
    uint64_t now = g_eds.get_tick();

    for (uint8_t i = 0; i < PERIOD_GROUP_MAX; i++)
    {
        period_group_t *grp = &g_eds.groups[i];

        /* 跳过未注册的组 */
        if (grp->events == NULL)
        {
            continue;
        }

        if (!grp->ready && !grp->pending && (now - grp->last_trigger) >= (uint64_t)grp->period)
        {
            grp->pending = 1;
            grp->last_trigger = now;
            grp->current_idx = 0;
        }
    }
}

/*******************************************************************************
 * 处理一轮通知事件：使用ctz_armcc快速定位最低位的pending bit
 * 先快照当前notify_pending并清零，本轮只处理快照中的bit
 * 执行期间新置的bit不会在本轮中提前执行，留到下一轮 
 *******************************************************************************/
static void process_notify_round(void)
{
    /* 快照当前pending，新触发写入notify_pending留到下一轮 */
    uint16_t pending = g_eds.notify_pending;
    g_eds.notify_pending = 0;

    while (pending)
    {
        /* __builtin_ctz: 计算末尾0的个数，即最低位1的位置 */
        uint8_t bit = (uint8_t)ctz_armcc(pending);
        pending &= ~(1u << bit);  /* 清除该bit */

        /* 执行回调 */
        notify_event_t *evt = &g_eds.notify_events[bit];
        if (evt->enabled && evt->callback)
        {
            evt->callback();
        }
    }
}

/*******************************************************************************
 * 执行一个时间任务：从current_group_idx开始轮转遍历周期组
 * 每次只执行一个回调，执行后记录当前组下标，下次继续从该组往后扫描
 * 这样避免10ms/20ms等短周期组反复占据执行机会，导致500ms/1000ms组饥饿
 * 返回值：1执行了一个时间任务，0无时间任务 
 *******************************************************************************/
static uint8_t process_one_time_event(void)
{
    /* 从上次执行到的组开始，轮转扫描所有组 */
    for (uint8_t scanned = 0; scanned < PERIOD_GROUP_MAX; scanned++)
    {
        uint8_t idx = (g_eds.current_group_idx + scanned) % PERIOD_GROUP_MAX;
        period_group_t *grp = &g_eds.groups[idx];

        /* 跳过未注册的组 */
        if (grp->events == NULL)
        {
            continue;
        }

        /* pending→ready：开始执行该组 */
        if (grp->pending && !grp->ready)
        {
            grp->ready = 1;
            grp->pending = 0;
            grp->current_idx = 0;
        }

        if (grp->ready && grp->current_idx < grp->event_count)
        {
            /* 执行当前回调（检查enabled） */
            if (grp->events[grp->current_idx].enabled && grp->events[grp->current_idx].callback)
            {
                grp->events[grp->current_idx].callback();
            }
            grp->current_idx++;

            /* 组内回调全部执行完毕，清ready */
            if (grp->current_idx >= grp->event_count)
            {
                grp->ready = 0;
            }

            /* 记录下次从该组的下一个组开始扫描 */
            g_eds.current_group_idx = (idx + 1) % PERIOD_GROUP_MAX;
            return 1;
        }
    }
    return 0;
}
