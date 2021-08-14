/*
 * File      : pm_button.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date             Author          Notes
 * 2017-11-05       realthread      the first version
 */

#pragma once

#include <pm_image.h>
#include <pm_widget.h>
#include <sigslot.h>

namespace Persimmon
{

class Button : public Widget
{
public:
    Button();
    Button(const Rect& rect);
    Button(const char *text, const Rect& rect);
    Button(Image *imgNor, Image *imgDown);
    Button(const char *text, Image *imgNor, Image *imgDown);
    virtual ~Button();

    void setText(const char *text);
    const char* getText(void);

    void setNorImage(Image *imgNor);
    Image* getNorImage(void);

    void setDownImage(Image *imgDown);
    Image* getDownImage(void);

    virtual std::string dumpValue() const { return text ? std::string(text) : std::string(); }

    void enableChange(bool enableChange = true);
    void enableLongpress(bool enableLongPress = true, rt_uint32_t longPressMs = 0, bool longPressOnce = false);

    Signal<int> clicked;

    virtual bool handleGestureEvent(struct rtgui_event_gesture *, const struct rtgui_gesture *);
    virtual void render(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

private:
    char *text;
    Image *imageNor, *imageDown;
    rt_tick_t tapTs, longPressTs;
    bool isDown, isLongPress, longPressOnce;
    bool enableChangeS, enableLongPress;
};

}
