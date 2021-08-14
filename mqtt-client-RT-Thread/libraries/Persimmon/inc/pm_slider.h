/*
 * File      : pm_slider.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */

#pragma once

#include <pm_widget.h>
#include <pm_image.h>
#include <sigslot.h>

namespace Persimmon
{

class Slider : public Widget
{
public:
    enum type
    {
        HORIZONTAL = 1 << 0,
        VERTICAL = 1 << 1,
        CYCLE = 1 << 2,
    };
    DEFINE_CLASS_ENUM_FLAG_OPERATORS(type);

    Slider();
    Slider(Image *norImg, Image *barImg);
    Slider(Image *norImg, Image *barImg, Image *sliderImg);
    virtual ~Slider();

    void setDirection(enum type t = HORIZONTAL)
    {
        direction = t;

        if (enableGes)
        {
            if (direction & HORIZONTAL)
                setInterestGesture(RTGUI_GESTURE_TAP | RTGUI_GESTURE_LONGPRESS | RTGUI_GESTURE_DRAG_HORIZONTAL);
            else
                setInterestGesture(RTGUI_GESTURE_TAP | RTGUI_GESTURE_LONGPRESS | RTGUI_GESTURE_DRAG_VERTICAL);
        }
    }

    void setValueLimits(int min, int max)
    {
        minValue = min;
        maxValue = max;
        if (maxValue <= minValue)
        {
            maxValue = minValue + 1;
        }
    }

    void setCurrentValue(int value)
    {
        if (value > maxValue)
            currentValue = maxValue;
        else if (value < minValue)
            currentValue = minValue;
        else
            currentValue = value;
        invalidate();
    }

    int getCurrentValue(void)
    {
        return currentValue;
    }

    void setNorImg(Image *img)
    {
        if (norImage)
        {
            delete norImage;
        }
        norImage = img;
    }

    void setBarImg(Image *img)
    {
        if (barImage)
        {
            delete barImage;
        }
        barImage = img;
    }

    void setSliderImg(Image *img)
    {
        if (sliderImage)
        {
            delete sliderImage;
        }
        sliderImage = img;
    }

    void setForegroundImg(Image *SImg, Image *EImg)
    {
        if (SImage) delete SImage;
        if (EImage) delete EImage;

        SImage = SImg;
        EImage = EImg;
    }

    void enableGesture(bool ges = true)
    {
        enableGes = ges;
        if (enableGes)
        {
            if (direction & HORIZONTAL)
                setInterestGesture(RTGUI_GESTURE_TAP | RTGUI_GESTURE_LONGPRESS | RTGUI_GESTURE_DRAG_HORIZONTAL);
            else
                setInterestGesture(RTGUI_GESTURE_TAP | RTGUI_GESTURE_LONGPRESS | RTGUI_GESTURE_DRAG_VERTICAL);
        }
        else
            setInterestGesture(RTGUI_GESTURE_NONE);
    }

    Signal<int, int> changed;

    virtual bool handleGestureEvent(struct rtgui_event_gesture *, const struct rtgui_gesture *); //触摸手势事件处理函数
    virtual void render(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

private:
    enum type direction;
    Image *norImage, *barImage, *sliderImage;
    Image *SImage, *EImage;
    int progress, baseProgress;
    int minValue, maxValue, currentValue;
    bool enableGes;
};

}
