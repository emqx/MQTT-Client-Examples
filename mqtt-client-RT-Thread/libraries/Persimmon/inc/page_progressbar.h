/*
 * File      : page_progressbar.h
 * COPYRIGHT (C) 2012-2018, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-09-01     XY           the first version
 */

#pragma once

#include <pm_page.h>
#include <resource.h>
#include <pm_progressbar.h>
#include <pm_label.h>

namespace Persimmon
{

class PageProgressBar : public Page
{
public:
    PageProgressBar(Window *win);
    virtual ~PageProgressBar();

    void onButton(int num);
    void onBtn(int num);

    ProgressBar *pbar1, *pbar2;
    int proV;

    Label *label1;
};

}
