/*
 * File      : pm_container.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */
#ifndef PM_CONTIANER_H__
#define PM_CONTIANER_H__

#include <pm_widget.h>
#include <rtgui/widgets/widget.h>
#include <rtgui/widgets/container.h>
#include "sigslot.h"
#include <vector>

namespace Persimmon
{

class Container : public Widget
{
    typedef Widget super;

public:
    Container(const rtgui_type_t* type = RTGUI_CONTAINER_TYPE);
    Container(const Rect &rect);
    virtual ~Container();

    virtual void addChild(Widget* widget);
    virtual void removeChild(Widget* widget);
    virtual void moveChildToTop(Widget* widget);
    virtual void moveChildToBottom(Widget* widget);
    virtual void moveChildUp(Widget* widget);
    virtual void moveChildDown(Widget* widget);
    virtual void moveChildAboveAt(Widget* widget, Widget* ref);
    virtual void moveChildBelowAt(Widget* widget, Widget* ref);

    int getChildIndex(Widget* widget);
    Widget * getChildbyName(const char *name, bool recursion = true);

    void removeAll();

    void saveClip(struct rtgui_region &region);
    void restoreClip(struct rtgui_region &region);

    virtual void setRect(const Rect &rect);

    void renderChildren(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);
    virtual void render(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);
    virtual rt_bool_t eventHandler(struct rtgui_event *event);

    virtual Widget* getMouseOwner(const rtgui_gesture *gesture);

    virtual std::string dumpWidgets() const;

protected:
    /* C++ children object under this window */
    std::vector<Widget*> children;

    void handleEventInChildren(struct rtgui_event *event);
};

}

#endif

