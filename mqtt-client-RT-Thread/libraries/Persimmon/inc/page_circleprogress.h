/*
 * File      : page_circleprogress.h
 * COPYRIGHT (C) 2012-2018, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-09-01     XY           the first version
 */

#pragma once

#include <pm_page.h>
#include <resource.h>
#include <circle_progress.h>
#include <pm_label.h>

namespace Persimmon
{

class PageCircleProgress : public Page
{
public:
    PageCircleProgress(Window *win);
    virtual ~PageCircleProgress();

    void onButton(int num);
    void onBtn(int num);

    CircleProgress *cprogress0, *cprogress1;
    unsigned int value;
    Label *label1;
};

}
