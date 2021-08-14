/*
 * File      : pm_colorblock.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */

#pragma once

#include <pm_widget.h>
#include <pm_image.h>

namespace Persimmon
{

class ColorBlock : public Widget
{
public:
    ColorBlock(const Rect &rect, rtgui_color_t color);
    virtual ~ColorBlock();

    virtual void render(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);
};

}
