/*
 * File      : page_card.h
 * COPYRIGHT (C) 2012-2018, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-09-01     XY           the first version
 */

#pragma once

#include <pm_page.h>
#include <resource.h>
#include <card.h>

namespace Persimmon
{

class PageCard : public Page
{
public:
    PageCard(Window *win);
    virtual ~PageCard();

    void onButton(int num);
    void onBtn(int num);

    Card *card;
};

}
