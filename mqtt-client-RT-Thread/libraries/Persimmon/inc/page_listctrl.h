/*
 * File      : page_listctrl.h
 * COPYRIGHT (C) 2012-2018, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-09-01     XY           the first version
 */

#pragma once

#include <pm_page.h>
#include <resource.h>
#include <pm_listctrl.h>

namespace Persimmon
{

class PageListCtrl : public Page
{
public:
    PageListCtrl(Window *win);
    virtual ~PageListCtrl();

    void onBtn(int num);
};

}
