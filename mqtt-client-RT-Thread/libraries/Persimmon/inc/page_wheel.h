/*
 * File      : page_wheel.h
 * COPYRIGHT (C) 2012-2018, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-09-01     XY           the first version
 */

#pragma once

#include <pm_page.h>
#include <resource.h>
#include <pm_wheel.h>
#include <pm_label.h>

namespace Persimmon
{

class PageWheel : public Page
{
public:
    PageWheel(Window *win);
    virtual ~PageWheel();

    void onWheelImage(int num);
    void onWheelString(int num);
    void onBtn(int num);

    Label *label1, *label2;
};

}
