/*
 * File      : circle_progress.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */

#pragma once

#include <pm_container.h>
#include <circle_dc.h>

namespace Persimmon
{

class CircleProgress: public Container
{
public:
    CircleProgress();
    virtual ~CircleProgress();

    void setStartAngle(unsigned int angle);
    void setEndAngle(unsigned int angle);
    void setProgressImage(Image *pro);
    void setProgressImage(struct rtgui_dc *pro);

    void setValueLimits(unsigned int minValue, unsigned int maxValue)
    {
        if (minValue > maxValue)
            return;

        this->minValue = minValue;
        this->maxValue = maxValue;
    }

    void setCircleR(unsigned int r1, unsigned int r2)
    {
        if (r1 > r2)
            return;

        this->r1 = r1;
        this->r2 = r2;
    }

    void setCurrentValue(unsigned int value)
    {
        if (value > maxValue)
            currentValue = maxValue;
        else if (value < minValue)
            currentValue = minValue;
        else
            currentValue = value;
        invalidate();
    }
    unsigned int getCurrentValue(void)
    {
        return currentValue;
    }

    void setCenter(int x, int y);
    virtual void renderBackground(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                                  const Rect &srcRect = Rect(), RenderFlag flags = DrawNormal);
    virtual void render(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

private:
    struct rtgui_dc *progressDc;
    unsigned int startAngle, endAngle, r1, r2;
    unsigned int currentValue, minValue, maxValue;
    struct rtgui_point center;
};

}
