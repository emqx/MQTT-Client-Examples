/*
 * File      : pm_timer.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */

#pragma once

#include <pm_widget.h>
#include <sigslot.h>

#include <rtgui/rtgui_system.h>

namespace Persimmon
{

class Timer : private utils::noncopyable<Timer>
{
public:
    enum flag
    {
        ONE_SHOT = RT_TIMER_FLAG_ONE_SHOT,
        PERIODIC = RT_TIMER_FLAG_PERIODIC,
        SOFT_TIMER = RT_TIMER_FLAG_SOFT_TIMER,
    };

    Timer(int time, enum flag flag);
    ~Timer();

    void start();
    void stop();
    void setTimeout(int time);
    int getTimerout() const;

    Signal<void> timeout;

private:
    struct rtgui_timer *m_timer;
    static void _onTimeout(struct rtgui_timer *timer, void *parameter);
};

class SyncTimer
{
    friend class Application;

public:
    enum Flag
    {
        ONE_SHOT = 0,
        PERIODIC = 1
    };
    SyncTimer(enum Flag flag = ONE_SHOT);
    ~SyncTimer();

    void start();
    void stop();

    Signal<void> timeout;

private:
    enum Status {
        START = 2
    };
    void handle();
    int m_flag;
};

} // namespace Persimmon
