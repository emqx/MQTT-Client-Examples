/*
 * File      : page_button.h
 * COPYRIGHT (C) 2012-2018, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-09-01     XY           the first version
 */

#pragma once

#include <pm_page.h>
#include <pm_label.h>

namespace Persimmon
{

class PageContainerButton : public Page
{
public:
    PageContainerButton(Window *win);
    virtual ~PageContainerButton();

    void onBtn(int num);
    void onButton(int num);

    Label *label1;
    int tapNum;
};

}
