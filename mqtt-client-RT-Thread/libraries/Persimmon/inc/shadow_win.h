/*
 * File      : shadow_win.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */

#pragma once

#include <pm_window.h>
#include <pm_container.h>

namespace Persimmon
{

class ShadowWin : public Window
{
public:
    ShadowWin(const Rect &rect, const char *title = "ShadowWin");
    virtual ~ShadowWin();

    virtual void addChild(Widget* widget);
    virtual void removeChild(Widget* widget);

    virtual int show(rt_bool_t isModal = RT_FALSE);

    void setBackground(rtgui_color_t color);
    void setBackground(Image *image);
    void setShadowColor(rtgui_color_t color)
    {
        shadowColor = color;
    }
    virtual void setRect(const Rect &rect)
    {
        panel->setRect(rect);
    }

protected:
    Container *panel;
    rtgui_color_t shadowColor;
};

}
