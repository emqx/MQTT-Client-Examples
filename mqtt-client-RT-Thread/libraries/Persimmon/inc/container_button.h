/*
 * File      : container_button.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */

#pragma once

#include <pm_container.h>

namespace Persimmon
{

class ContainerButton : public Container
{
public:
    ContainerButton(const Rect &rect);
    ContainerButton(const Rect &rect, Image *imgNor, Image *imgDown);
    virtual ~ContainerButton();

    void setNorImage(Image *imgNor);
    void setNorImage(struct rtgui_dc *imgNor);

    void setDownImage(Image *imgDown);
    void setDownImage(struct rtgui_dc *imgDown);

    void enableLongpress(bool enableLongPress = true, rt_uint32_t longPressMs = 0, bool longPressOnce = false);

    Signal<int> clicked;

    virtual bool handleGestureEvent(struct rtgui_event_gesture *, const struct rtgui_gesture *);
    virtual void render(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

private:
    struct rtgui_dc* loadImageDc(Image *image);

    struct rtgui_dc *imageNor, *imageDown;
    rt_tick_t tapTs, longPressTs;
    bool isDown, isLongPress, longPressOnce;
};

}
