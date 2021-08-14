/*
 * File      : page_numerical.h
 * COPYRIGHT (C) 2012-2018, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-09-01     XY           the first version
 */

#pragma once

#include <pm_page.h>
#include <resource.h>
#include <pm_numerical.h>

namespace Persimmon
{

class PageNumerical : public Page
{
public:
    PageNumerical(Window *win);
    virtual ~PageNumerical();

    void onButton(int num);
    void onBtn(int num);

    Numerical *num1, *num2;
    double value1, value2;
};

}
