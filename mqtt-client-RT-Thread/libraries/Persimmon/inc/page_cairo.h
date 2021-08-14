/*
 * File      : page_cairo.h
 * COPYRIGHT (C) 2012-2018, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-09-01     XY           the first version
 */

#pragma once

#include <pm_page.h>
#include <resource.h>
#include <pm_cairo.h>
#ifdef PKG_USING_CAIRO
namespace Persimmon
{

class PageCairo : public Page
{
public:
    PageCairo(Window *win);
    virtual ~PageCairo();

    void onButton(int num);
    void onBtn(int num);

    Cairo *cairo;
};

}
#endif
