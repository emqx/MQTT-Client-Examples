/*
 * File      : page_label.h
 * COPYRIGHT (C) 2012-2018, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-09-01     XY           the first version
 */

#pragma once

#include <pm_page.h>
#include <resource.h>

namespace Persimmon
{

class PageLabel : public Page
{
public:
    PageLabel(Window *win);
    virtual ~PageLabel();

    void onBtn(int num);
};

}
