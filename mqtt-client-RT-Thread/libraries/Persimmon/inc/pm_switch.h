/*
 * File      : pm_switch.h
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
#include <sigslot.h>
#include <pm_timer.h>

namespace Persimmon
{

class Switch : public Widget
{
public:
    Switch();
    Switch(const Rect &rect);
    Switch(Image* offBgImg, Image* onBgImg, bool status = false);
    virtual ~Switch();

    void setBgImage(Image *offBgImg, Image *onBgImg);
    void setOnImage(Image *image);
    void setOffImage(Image *image);

    void setStatus(bool on);
    bool getStatus(void);

    void setText(const char *text);
    char *getText(void);

    virtual std::string dumpValue() const { return text ? std::string(text) : std::string(); }

    Signal<int, bool> changed;

    virtual bool handleGestureEvent(struct rtgui_event_gesture *, const struct rtgui_gesture *);
    virtual void render(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

private:
    Image *offBgImage, *onBgImage;
    bool on;
    char *text;
};

}
