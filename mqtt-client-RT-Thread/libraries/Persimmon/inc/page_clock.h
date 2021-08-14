/*
 * File      : page_clock.h
 * COPYRIGHT (C) 2012-2018, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-09-01     XY           the first version
 */

#pragma once

#include <pm_page.h>
#include <resource.h>
#include <pm_clock.h>
#include <pm_timer.h>

namespace Persimmon
{

class PageClock : public Page
{
public:
    PageClock(Window *win);
    virtual ~PageClock();

    void onTimer(void);
    void onBtn(int num);

    Timer *cTimer;
    Clock *clock;
    int time;
};

}
