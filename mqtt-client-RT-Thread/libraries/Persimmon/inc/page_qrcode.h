/*
 * File      : page_qrcode.h
 * COPYRIGHT (C) 2012-2018, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-09-01     XY           the first version
 */

#pragma once

#include <pm_page.h>
#include <resource.h>
#include <pm_qrcode.h>

namespace Persimmon
{

class PageQRCode : public Page
{
public:
    PageQRCode(Window *win);
    virtual ~PageQRCode();

    void onBtn(int num);
};

}
