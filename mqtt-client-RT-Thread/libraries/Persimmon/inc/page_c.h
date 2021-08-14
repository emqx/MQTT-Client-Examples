/*
 * File      : page_c.h
 * COPYRIGHT (C) 2012-2018, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-03-06     realthread   the first version
 */

#pragma once

#include <pm_page.h>

namespace Persimmon
{

class PageC : public Page
{
public:
    PageC(Window *win);
    virtual ~PageC();

    void onBtn(int num);
};

}
