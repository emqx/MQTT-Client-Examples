/*
 * File      : pm_panel.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */

#pragma once

#include <pm_container.h>
#include <pm_image.h>

namespace Persimmon
{

class Panel : public Container
{
public:
    enum type
    {
        NONE = 0x00,
        HORIZONTAL = 1 << 0,
        VERTICAL   = 1 << 1,
        CYCLE = 1 << 2,
        LEFT = 1 << 3,
        TOP = 1 << 4,
    };
    DEFINE_CLASS_ENUM_FLAG_OPERATORS(type);

    Panel();
    Panel(const Rect rect, enum type tp = VERTICAL);
    Panel(const Panel &other);
    virtual ~Panel();

    virtual Widget *depthCopy(void) const;
    void setDirection(enum type direction);

    void resetConfig(void);
    void setScrollbar(Image *yImg, Image *xImg);
    void addItem(Widget *w, bool relative = false);
    void fixItemExtent(int xOffset, int yOffset);

    virtual bool handleGestureEvent(struct rtgui_event_gesture *gev,
                                    const struct rtgui_gesture *gest);

    virtual void render(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

private:
    enum type direction;

    int yLastPosition, yFirstPosition;
    int xLastPosition, xFirstPosition;
    int yMovePitch, xMovePitch;

    Image *yImage, *xImage;
};

}
