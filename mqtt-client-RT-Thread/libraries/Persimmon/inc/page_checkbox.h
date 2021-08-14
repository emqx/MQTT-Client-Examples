/*
 * File      : page_checkbox.h
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

class PageCheckbox : public Page
{
public:
    PageCheckbox(Window *win);
    virtual ~PageCheckbox();

    void onCheckBox(int num, bool state);
    void onBtn(int num);

    Label *label1, *label2;
};

}
