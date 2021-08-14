/*
 * File      : page_switch.h
 * COPYRIGHT (C) 2012-2018, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-09-01     XY           the first version
 */

#pragma once

#include <pm_page.h>
#include <resource.h>
#include <pm_label.h>

namespace Persimmon
{

class PageSwitch : public Page
{
public:
    PageSwitch(Window *win);
    virtual ~PageSwitch();

    void onSwitch(int num, bool state);
    void onBtn(int num);

    Label *label1, *label2, *label3;
};

}
