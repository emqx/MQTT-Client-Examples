/*
 * File      : pm_widget.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */

#pragma once

#include <rtgui/rtgui.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/widgets/widget.h>
#include <gesture.h>

#include <persimmon.h>
#include <pm_rect.h>
#include <pm_image.h>

#include <string>

namespace Persimmon
{
namespace utils
{

/* Borrowed from Boost::noncopyable. Add the template to give each noncopyable
* a different signature. */
template <typename T>
class noncopyable
{
protected:
    noncopyable() {}
    ~noncopyable() {}
private:  // emphasize the following members are private
    noncopyable( const noncopyable& );
    noncopyable& operator=( const noncopyable& );
};

#define DEFINE_CLASS_ENUM_FLAG_OPERATORS(ENUMTYPE) \
    friend inline ENUMTYPE operator | (ENUMTYPE a,  ENUMTYPE b) \
           { return ENUMTYPE(((int)a)|((int)b)); } \
    friend inline ENUMTYPE operator |= (ENUMTYPE &a,  ENUMTYPE b) \
           { return (ENUMTYPE &)(((int &)a) |= ((int)b)); } \
    friend inline ENUMTYPE operator & (ENUMTYPE a,  ENUMTYPE b) \
           { return ENUMTYPE(((int)a)&((int)b)); } \
    friend inline ENUMTYPE operator &= (ENUMTYPE &a,  ENUMTYPE b) \
           { return (ENUMTYPE &)(((int &)a) &= ((int)b)); } \
    friend inline ENUMTYPE operator ~ (ENUMTYPE a) \
           { return (ENUMTYPE)(~((int)a)); } \
    friend inline ENUMTYPE operator ^ (ENUMTYPE a,  ENUMTYPE b) \
           { return ENUMTYPE(((int)a)^((int)b)); } \
    friend inline ENUMTYPE operator ^= (ENUMTYPE &a,  ENUMTYPE b) \
           { return (ENUMTYPE &)(((int &)a) ^= ((int)b)); }

#define DEFINE_CLASS_ENUM_FLAG_OPERATORS2(ENUMTYPE) \
    inline ENUMTYPE operator | (ENUMTYPE a,  ENUMTYPE b) \
           { return ENUMTYPE(((int)a)|((int)b)); } \
    inline ENUMTYPE operator |= (ENUMTYPE &a,  ENUMTYPE b) \
           { return (ENUMTYPE &)(((int &)a) |= ((int)b)); } \
    inline ENUMTYPE operator & (ENUMTYPE a,  ENUMTYPE b) \
           { return ENUMTYPE(((int)a)&((int)b)); } \
    inline ENUMTYPE operator &= (ENUMTYPE &a,  ENUMTYPE b) \
           { return (ENUMTYPE &)(((int &)a) &= ((int)b)); } \
    inline ENUMTYPE operator ~ (ENUMTYPE a) \
           { return (ENUMTYPE)(~((int)a)); } \
    inline ENUMTYPE operator ^ (ENUMTYPE a,  ENUMTYPE b) \
           { return ENUMTYPE(((int)a)^((int)b)); } \
    inline ENUMTYPE operator ^= (ENUMTYPE &a,  ENUMTYPE b) \
           { return (ENUMTYPE &)(((int &)a) ^= ((int)b)); }
}

class Container;
class Window;

/**
 * Persimmon UI Widget
 */
class Widget: private utils::noncopyable<Widget>
{
public:
    enum WidgetFlags
    {
        WidgetVisiable = RTGUI_WIDGET_FLAG_SHOWN,
        WidgetDisable = RTGUI_WIDGET_FLAG_DISABLE,
        WidgetFocus = RTGUI_WIDGET_FLAG_FOCUS,
        WidgetTransparent = RTGUI_WIDGET_FLAG_TRANSPARENT,
        WidgetFocusable = RTGUI_WIDGET_FLAG_FOCUSABLE,
        WidgetDcVisiable = RTGUI_WIDGET_FLAG_DC_VISIBLE,
        WidgetInAnim = RTGUI_WIDGET_FLAG_IN_ANIM,
        WidgetDestoryLater = 0x400
    };

    Widget(const rtgui_type_t *widget_type = RTGUI_WIDGET_TYPE);
    Widget(const Widget &other);
    virtual ~Widget();

    virtual Widget* depthCopy(void) const;

    void destoryLater();

    const rtgui_type_t* getType() const
    {
        return widget->object.type;
    }

    Widget* setFont(struct rtgui_font* font)
    {
        RTGUI_WIDGET_FONT(this->widget) = font;
        invalidate();
        return this;
    }
    Widget* setFont(int size);

    struct rtgui_font* getFont(void)
    {
        return RTGUI_WIDGET_FONT(this->widget);
    }

    void setBackground(rtgui_color_t color);
    void setBackground(Image *image);
    void setBackground(struct rtgui_dc* bg);
    void setTransparentBackground(Image *image);
    struct rtgui_dc* getBackground(void) {
        return backgroundDc;
    }

    rtgui_color_t backgroundColor() const
    {
        return RTGUI_WIDGET_BACKGROUND(widget);
    }
    rtgui_color_t foregroundColor() const
    {
        return RTGUI_WIDGET_FOREGROUND(widget);
    }

    void setProvisionalBackground(struct rtgui_dc* bg)
    {
        provisionalBgDc = bg;
        invalidate();
    }

    void setForeground(rtgui_color_t color)
    {
        RTGUI_WIDGET_FOREGROUND(this->widget) = color;
        invalidate();
    }

    void setTextAlign(int align)
    {
        RTGUI_WIDGET_TEXTALIGN(widget) = align;
        invalidate();
    }

    struct rtgui_widget* getWidget(void)
    {
        return RTGUI_WIDGET(this->widget);
    }

    Rect getRect(void) const;
    void getRect(struct rtgui_rect* rect) const;
    virtual void setRect(const Rect &rect);
    void setRect(const rtgui_rect_t *rect)
    {
        setRect(Rect(rect));
    }
    void setRect(int x, int y, int width, int height)
    {
        setRect(Rect(x, y, width, height));
    }
    void setPosition(const Point &position);
    void setPosition(int x, int y)
    {
        setPosition(Point(x, y));
    }
    void setWidth(int width);
    void setHeight(int height);
    void setPositionX(int pos);
    void setPositionY(int pos);

    int getWidth() const;
    int getHeight() const;
    int getPositionX() const;
    int getPositionY() const;
    Point getAbsPosition() const;  /* get the absolute position of the widget */
    Point getPosition(void) const; /* get the position of the widget relative to the parent */
    Size getSize(void) const;
    void setSize(const Size &size);
    Rect getGeometry() const;      /* get the shape rectangle relative to the position of the parent */
    Rect getExtent(void) const;    /* get the shape rectangle of the absolute position */
    void getExtent(struct rtgui_rect* rect) const;

    Widget* getVisiableParent(void);

    bool isInAnimation(void)
    {
        return rtgui_widget_is_in_animation(getWidget()) ? true : false ;
    }

    bool isTransparent(void)
    {
        return widget->flag & RTGUI_WIDGET_FLAG_TRANSPARENT ? true : false;
    }

    bool isExposed();

    void refresh(bool update = true);

    void invalidate(const Rect &rect);
    void invalidate()
    {
        invalidate(getRect());
    }
    rt_bool_t isInvalid();
    Rect invalidRect();
    void clearInvalid();
    void updateInvalid();
    void repaint(void);

    void setOpacity(int opacity);
    int opacity()
    {
        return m_opacity;
    }

    void freeze(void);
    void thaw(void);

    void setLastFocus(bool focus);
    void move(int x, int y);
    void setAbsPosition(int x, int y);

    virtual void render(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(), RenderFlag flags = DrawNormal);
    virtual void renderBackground(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                                  const Rect &srcRect = Rect(), RenderFlag flags = DrawNormal);

    struct rtgui_dc* getBufferDrawing(rt_uint8_t pixel_format, RenderFlag flags = DrawNormal, const Rect &rect = Rect() );
    struct rtgui_dc* getBufferDrawing(RenderFlag flags = DrawNormal);
    void bufferDrawing(struct rtgui_dc_buffer *buf, RenderFlag flags = DrawNormal, const Rect& rect = Rect());

    int flags() const;
    void setFlags(int flags, bool enabled = true);
    bool isVisiable() const;
    void hide();
    void show();
    virtual rt_bool_t onHide(struct rtgui_event *event);
    virtual rt_bool_t onShow(struct rtgui_event *event);

    virtual rt_bool_t eventHandler(struct rtgui_event *event);

    virtual Widget* getMouseOwner(const rtgui_gesture *gesture);

    virtual std::string dumpWidgets() const;
    std::string escapeDumpValue() const;

    void setInterestGesture(enum rtgui_gesture_type tp)
    {
        gest_tp = tp;
    }
    enum rtgui_gesture_type interestGesture()
    {
        return gest_tp;
    }
    enum rtgui_gesture_type getInterestGesture();

    virtual bool handleGestureEvent(struct rtgui_event_gesture *gev,
                                    const struct rtgui_gesture *gest);

    Widget* getParent() const
    {
        return parent;
    }

    void setParent(Widget *w)
    {
        parent = w;
        rtgui_widget_set_parent(widget, w ? w->widget : NULL);
    }

    virtual void setGestureRect(const Rect &rect, bool enable = true)
    {
        gestureRect = rect;
        enableGestureRect = enable;
    }

    void renderFloating(struct rtgui_dc *dc);

    void setNumber(int num)
    {
        number = num;
    }
    int getNumber(void)
    {
        return number;
    }

    const std::string &getName(void)
    {
        return name;
    }
    void setName(std::string &str)
    {
        name = str;
    }

    Widget* getPageWidgetByName(const char *name);

    void setTypeName(const char *str) { m_typeName = str; }
    const char* typeName() const { return m_typeName; }

    virtual std::string dumpValue() const { return std::string(); }

    virtual void setData(void *data);
    virtual void getData(void *ret, const std::string &name) const;

protected:
    static void renderParentBackground(Widget* widget, Widget* parent, struct rtgui_dc* dc, const Point &dcPoint, const Rect &srcRect);

    struct rtgui_widget* widget;
    struct rtgui_dc *backgroundDc, *provisionalBgDc;
    std::string name;

private:
    void renderOpacity(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                       const Rect &srcRect = Rect(), RenderFlag flags = DrawNormal);

private:
    /* Set by Container::addChild. */
    Widget *parent;

    bool enableGestureRect;
    Rect gestureRect;
    Rect m_invalidRect;

    friend class Window;
    enum rtgui_gesture_type gest_tp;
    int number;
    int m_opacity;
    const char *m_typeName;
};

}

DEFINE_CLASS_ENUM_FLAG_OPERATORS2(rtgui_gesture_type);
