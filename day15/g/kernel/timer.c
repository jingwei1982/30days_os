#include "bootpack.h"

void init_pit(void)
{
    int i;
    struct TIMER *t;
    io_out8(PIT_CTRL, 0x34);
    io_out8(PIT_CNT0, 0x9c);
    io_out8(PIT_CNT0, 0x2e);
    timerctl.count = 0;
    for (i = 0; i < MAX_TIMER; i++)
    {
        timerctl.timers0[i].flags = 0;
    }
    t = timer_alloc();
    t->timeout = 0xffffffff;
    t->flags = TIMER_FLAGS_USING;
    t->next_timer = 0;
    timerctl.t0 = t;
    timerctl.next_time = 0xffffffff;
    return;
}

struct TIMER *timer_alloc(void)
{
    int i;
    for (i = 0; i < MAX_TIMER; i++)
    {
        if (timerctl.timers0[i].flags == 0)
        {
            timerctl.timers0[i].flags = TIMER_FLAGS_ALLOC;
            return &timerctl.timers0[i];
        }
    }
    return 0;
}

void timer_free(struct TIMER *timer)
{
    timer->flags = 0;
    return;
}

void timer_init(struct TIMER *timer, struct FIFO32 *fifo, unsigned char data)
{
    timer->fifo = fifo;
    timer->data = data;
    return;
}

void timer_settime(struct TIMER *timer, unsigned int timeout)
{
    int e;
    struct TIMER *t, *s;
    timer->timeout = timeout + timerctl.count;
    timer->flags = TIMER_FLAGS_USING;
    e = io_load_eflags();
    io_cli();
    t = timerctl.t0;
    if (timer->timeout <= t->timeout)
    {
        /* 插入最前面的情况 */
        timerctl.t0 = timer;
        timer->next_timer = t; /* 下面是设定t */
        timerctl.next_time = timer->timeout;
        io_store_eflags(e);
        return;
    }
    for (;;)
    {
        s = t;
        t = t->next_timer;
        if (timer->timeout <= t->timeout)
        {
            /* 插入s和t之间的情况 */
            s->next_timer = timer; /* s下一个是timer */
            timer->next_timer = t; /* timer的下一个是t */
            io_store_eflags(e);
            return;
        }
    }
}

void inthandler20(int *esp)
{
    char ts = 0;
    struct TIMER *timer;
    io_out8(PIC0_OCW2, 0x60); /* 把IRQ-00接收信号结束的信息通知给PIC */
    timerctl.count++;
    if (timerctl.next_time > timerctl.count)
    {
        return;
    }
    timer = timerctl.t0; /* 首先把最前面的地址赋给timer */
    for (;;)
    {
        /* 因为timers的定时器都处于运行状态，所以不确认flags */
        if (timer->timeout > timerctl.count)
        {
            break;
        }
        /* 超时 */
        timer->flags = TIMER_FLAGS_ALLOC;
        if(timer!=mt_timer)
        {
            fifo32_put(timer->fifo, timer->data);
        }
        else
        {
            ts = 1; //mt_timer超时
        }
        timer = timer->next_timer; /* 将下一个定时器的地址赋给timer*/
    }
    timerctl.t0 = timer;
    timerctl.next_time = timer->timeout;
    if(ts!=0)   //等中断处理完成，再切换。否则因为mt_taskswitch切换时候，即便中断没处理完，IF也会设为1，如果这时候有新中断，会出错。
    {
        mt_taskswitch();
    }
    return;
}