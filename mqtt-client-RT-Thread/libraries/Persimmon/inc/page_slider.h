/*
 * File      : page_slider.h
 * COPYRIGHT (C) 2012-2018, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-09-01     XY           the first version
 */

#pragma once

#include <pm_page.h>
#include <resource.h>
#include <pm_slider.h>
#include <pm_label.h>

namespace Persimmon
{

class PageSlider : public Page
{
public:
    PageSlider(Window *win);
    virtual ~PageSlider();

    void onSlider(int num, int value);
    void onButton(int num);
    void onBtn(int num);

    Slider *pbar1, *pbar2;
    int proV;

    Label *label1;
};

}
