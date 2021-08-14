/*
 * File      : pm_numerical.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */

#pragma once

#include <vector>
#include <pm_widget.h>
#include <pm_image.h>
#include "resource.h"

namespace Persimmon
{

class Numerical : public Widget
{
public:
    enum type
    {
        ALIGN_LEFT = 0x00,
        ALIGN_RIGHT = 0x01,
    };

    Numerical();
    Numerical(const Rect& rect);
    virtual ~Numerical();

    void addImage(Image *img)
    {
        if (img)
            image.push_back(img);
    }

    void setNumGap(unsigned int gap)
    {
        this->gap = gap;
    }

    void setIntegerDigits(unsigned int digits)
    {
        this->integerDigits = digits;
    }

    void setDecimalDigits(unsigned int digits)
    {
        this->decimalDigits = digits;
    }

    void setUnit(Image *image)
    {
        if (unit)
            delete unit;

        unit = image;
    }

    void setAlign(enum type align)
    {
        this->align = align;
    }

    void setNumerical(double value);

    virtual void render(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

private:
    enum type align;
    std::vector<Image *> image;
    Image *unit;
    double numerical;
    unsigned int integerDigits, decimalDigits, gap;
};

}
