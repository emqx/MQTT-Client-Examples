/*
 * File      : pm_widgetwheel.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */

#pragma once

#include <vector>

#include <pm_container.h>
#include <sigslot.h>
#include <pm_timer.h>

namespace Persimmon
{

class WidgetWheel : public Container
{
public:
    enum type
    {
        HORIZONTAL = 1 << 0,
        VERTICAL   = 1 << 1,
        CYCLE      = 1 << 2,
    };
    DEFINE_CLASS_ENUM_FLAG_OPERATORS(type);

    WidgetWheel(const Rect& rect, int norSize, int selSize);
    virtual ~WidgetWheel();

    void addItem(Widget* widget);
    void emptyItem(void);

    void setSlideType(enum type tp = HORIZONTAL)
    {
        mtype = tp;

        if (mtype & HORIZONTAL)
            setInterestGesture(RTGUI_GESTURE_TAP | RTGUI_GESTURE_LONGPRESS | RTGUI_GESTURE_DRAG_HORIZONTAL);
        else
            setInterestGesture(RTGUI_GESTURE_TAP | RTGUI_GESTURE_LONGPRESS | RTGUI_GESTURE_DRAG_VERTICAL);
    }

    void fixItemExtent(int itemNum, bool ref);

    int getSelItemNum(void)
    {
        return selItemNum;
    }

    Widget* getSelItem(void)
    {
        return item[selItemNum];
    }

    Widget* getItem(int value)
    {
        if (value < 0 || value >= (int)item.size())
            return RT_NULL;

        return item[value];
    }

    void setAdjustSpeed(int value)
    {
        if (value >= 0)
            adjustSpeed = value;
    }

    void animationEnable(bool enable = true)
    {
        animEnable = enable;
    }

    void tapSelEnable(bool enable = true)
    {
        tapEnable = enable;
    }

    Signal<int> clicked;

    virtual void act(int itemNum, int progress);
    virtual bool handleGestureEvent(struct rtgui_event_gesture *gev, const struct rtgui_gesture *gest);

protected:
    std::vector<Widget*> item;
    std::vector<int> item_num;
    int norExtentSize;
    enum type mtype;

private:
    void moveItem(bool ref);
    void filterSelItem(void);

    void animationStart(void);
    void onAnimation(void);

    int selItemNum, oldSelItemNum, selExtentSize;
    int itemMovePitch, oldPitch, adjustSpeed;

    Timer *animationTimer;
    bool animationMoving, animEnable, tapEnable;
    int animationMovePitch, _progress;
    int animationPitch, animationProgress;
};

}
