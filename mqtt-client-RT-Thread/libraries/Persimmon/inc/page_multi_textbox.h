/*
 * File      : page_multi_textbox.h
 * COPYRIGHT (C) 2012-2018, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-09-01     XY           the first version
 */

#pragma once

#include <pm_page.h>
#include <resource.h>
#include <pm_multi_textbox.h>

namespace Persimmon
{

class PageMultiTextBox : public Page
{
public:
    PageMultiTextBox(Window *win);
    virtual ~PageMultiTextBox();

    void onBtn(int num);
};

}
