/*
 * File      : page_dotIndicator.h
 * COPYRIGHT (C) 2012-2018, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-09-01     XY           the first version
 */

#pragma once

#include <pm_page.h>
#include <resource.h>
#include <pm_dotIndicator.h>

namespace Persimmon
{

class PageDotIndicator : public Page
{
public:
    PageDotIndicator(Window *win);
    virtual ~PageDotIndicator();

    void onButton(int num);
    void onBtn(int num);

    DotIndicator *dot, *dot1;
    int dotSel;
};

}
