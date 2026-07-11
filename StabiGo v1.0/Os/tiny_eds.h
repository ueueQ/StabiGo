#ifndef __TINY_EDS_H
#define __TINY_EDS_H

#include <stdint.h>

/*============================================================================
 * 宏定义
 *============================================================================*/

#define ctz_armcc(x) __clz(__rbit(x))

#define TIME_EVENT_MAX        8      /* 最大时间事件数量 */
#define NOTIFY_EVENT_MAX      16     /* 最大通知事件数量，对应uint16的16个bit */
#define PERIOD_GROUP_MAX      7      /* 周期组数量，对应7个固定周期 */

#define REGISTER_MS_EVENT(period, event)  register_time_event(period, event, sizeof(event) / sizeof(time_event_t))
#define REGISTER_NOTIFY_EVENT(event)  register_notify_event(event, sizeof(event) / sizeof(notify_event_t))


/*============================================================================
 * 时间周期枚举
 * 只允许注册以下固定周期，不可设置其他值
 *============================================================================*/
typedef enum {
    PERIOD_10MS   = 10,     /* 10ms周期 */
    PERIOD_20MS   = 20,     /* 20ms周期 */
    PERIOD_50MS   = 50,     /* 50ms周期 */
    PERIOD_100MS  = 100,    /* 100ms周期 */
    PERIOD_200MS  = 200,    /* 200ms周期 */
    PERIOD_500MS  = 500,    /* 500ms周期 */
    PERIOD_1000MS = 1000,   /* 1000ms周期 */
} time_period_t;

/*============================================================================
 * 时间事件结构体
 * 用于存储时间驱动事件的具体配置和状态
 * 用户通过此结构体逐个注册时间事件，内部自动归入对应周期组
 *============================================================================*/
typedef struct {
    void (*callback)(void);          /* 回调函数指针，时间到达时执行的函数 */
    uint8_t enabled;                 /* 使能标志，0:禁用 1:使能 */
} time_event_t;

/*============================================================================
 * 周期组结构体
 * 按固定周期分组，每组独立追踪last_trigger，到期时O(1)触发
 * 7个固定组，内部使用，用户不直接操作
 *============================================================================*/
typedef struct {
    time_period_t period;                        /* 该组的周期值 */
    uint64_t last_trigger;                       /* 上次触发时的tick，now-last_trigger>=period则到期 */
    uint8_t pending;                             /* 到期待处理标志，到期时置1 */
    uint8_t ready;                               /* 正在执行标志，pending→ready开始执行，组内回调全部执行完清零 */
    time_event_t *events;                        /* 指向用户注册的事件数组 */
    uint8_t event_count;                         /* 事件数组长度 */
    uint8_t current_idx;                         /* 当前执行到的回调下标，每次执行一个后+1 */
} period_group_t;

/*============================================================================
 * 通知事件结构体
 * 用于存储通知驱动事件的具体配置和状态
 * 通知触发通过uint16位域实现，无需FIFO队列
 *============================================================================*/
typedef struct {
    void (*callback)(void);          /* 回调函数指针，通知触发时执行的函数 */
    uint8_t enabled;                 /* 使能标志，0:禁用 1:使能 */
} notify_event_t;

/*============================================================================
 * TinyEDS 核心结构体
 * 包含周期组数组和通知事件数组
 *
 * 时间调度：7个周期组，每组独立追踪last_trigger，到期时O(1)触发
 * 通知优先规则：
 *   - notify_pending为uint16位域，每个bit对应一个通知事件
 *   - bit=1表示该事件待执行，bit位越低优先级越高
 *   - 通知驱动优先于时间驱动，当前任务执行完毕后立即检测通知
 *============================================================================*/
typedef struct {
    /* 周期组 */
    uint8_t current_group_idx;                   /* 当前执行到的周期组下标，轮转防饥饿 */
    period_group_t groups[PERIOD_GROUP_MAX];     /* 7个周期组，按period从小到大排列 */

    /* 通知事件 */
    notify_event_t *notify_events;                   /* 指向用户注册的通知事件数组，下标即优先级(bit0最高) */
    uint8_t notify_event_count;                      /* 通知事件数组长度 */
    volatile uint16_t notify_pending;                /* 通知待处理位域，bit0=最高优先级 */

    /* 系统滴答回调，用户实现，返回当前系统ms计数 */
    uint64_t (*get_tick)(void);

    /* 调度器状态 */
    uint8_t is_running;              /* 调度器运行标志，0:停止 1:运行中 */
} tiny_eds_t;

/*============================================================================
 * API函数声明
 *============================================================================*/

/* 初始化TinyEDS，传入用户实现的get_tick回调 */
void tiny_eds_init(uint64_t (*get_tick)(void));

/* 注册时间事件，指定周期和事件数组，自动归入对应周期组，返回0成功-1失败 */
int8_t register_time_event(time_period_t period, time_event_t evts[], uint8_t count);

/* 注册通知事件，传入事件数组，数组下标即优先级(bit0最高)，返回0成功-1失败 */
int8_t register_notify_event(notify_event_t evts[], uint8_t count);

/* 触发通知事件（中断中调用），将对应bit置1 */
void notify_event_trigger(uint8_t event_id);

/* TinyEDS运行，主循环中调用 */
void tiny_eds_run(void);

/* TinyEDS停止 */
void tiny_eds_stop(void);

#endif /* __TINY_EDS_H */
