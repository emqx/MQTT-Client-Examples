/*
 * File      : pm_listctrl.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */

#pragma once

#include <vector>
#include <pm_container.h>
#include <pm_animation.h>
#include <pm_image.h>

namespace Persimmon
{

class AbstractBar;
class IntAnimator;
class AbstractInterpolator;
class ListInterpolator;

class ListCtrl : public Container
{
public:
    enum type
    {
        NONE = 0x00,
        HORIZONTAL = 1 << 0,
        VERTICAL = 1 << 1,
        CYCLE = 1 << 2,
        LEFT = 1 << 3,
        TOP = 1 << 4,
    };
    enum ItemAlignment
    {
        AlignNone,
        AlignTop,
        AlignCenter
    };
    DEFINE_CLASS_ENUM_FLAG_OPERATORS(type);
    enum MoveType
    {
        DropStart,
        DropFirst,
        DropLast,
        MoveChange
    };

    ListCtrl();
    ListCtrl(const Rect &rect);
    ListCtrl(const ListCtrl &other);
    virtual ~ListCtrl();

    virtual Widget *depthCopy(void) const;
    void setScrollBar(AbstractBar *bar);
    void setScrollbar(Image *img);
    void setScrollbarBg(Image *img);
    void setScrollbarPosition(int p);
    void setBoundaryImg(Image *top, Image *bottom);
    void addItem(Widget *widget);
    void addItem(int index, Widget *widget, bool upthrust = false);
    void removeItem(Widget *widget);
    void removeItem(int index);
    void removeItem(const char *name);
    void emptyItem(void);

    void setDirection(enum type direction);
    void setItemGap(int gap)
    {
        if (gap < 0)
            return;

        itemGap = gap;
        firstPosition = 0;
        lastPosition = 0;
    }

    void setAcceleration(unsigned int value)
    {
        acceleration = value > 100 ? value : 100;
    }

    void movePosition(int position);
    void moveToPosition(int position, bool anim = false);
    void fixItemExtent(bool force = false);
    void setItemAlign(ItemAlignment align);
    void setItemWidth(int width);
    void setItemHeight(int height);
    void selectIndex(int index);
    int focusIndex() const;

    virtual rt_bool_t onHide(struct rtgui_event *event);
    virtual rt_bool_t onShow(struct rtgui_event *event);
    virtual bool handleGestureEvent(struct rtgui_event_gesture *gev,
                                    const struct rtgui_gesture *gest);

    virtual void render(struct rtgui_dc *dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

    virtual Widget* getMouseOwner(const rtgui_gesture *gesture);

    Signal<MoveType, int> bindSignal;

private:
    void fixItemExtent(Widget *widget, bool force = false);
    bool startAnimation(int speed = 0);
    void updateScrollBar();
    void onAnimRun(int value);
    void onAnimFinish();
    ListInterpolator *m_interpolator;

protected:

    int firstPosition, lastPosition;
    int itemGap;
    enum type direction;
    IntAnimator *m_anim;
    int animMovePitch;
    unsigned int acceleration;
    int scrollbarPosition;
    bool isTop, isBottom, showScrollbar, inGesture;
    Image *scrollbar, *scrollbarBg, *boundaryLT, *boundaryRB;
    AbstractBar *m_scrollBar;
    int m_itemWidth, m_itemHeight;
    ItemAlignment m_itemAlign;
};

} // namespace Persimmon
