/*
 * File      : pm_wheel.h
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

class IntAnimator;

class Wheel : public Container
{
public:
    enum type
    {
        HORIZONTAL = 1 << 0,
        VERTICAL = 1 << 1,
    };
    DEFINE_CLASS_ENUM_FLAG_OPERATORS(type);

    Wheel(enum type tp = HORIZONTAL);
    Wheel(const Rect& rect, enum type tp = HORIZONTAL);
    virtual ~Wheel();

    void fixSelItem(void);
    void fixSelItem(unsigned int num);
    void fixItemPtich(bool actEnable = true);
    void selectItem(unsigned int num);

    virtual bool handleGestureEvent(struct rtgui_event_gesture *gev, const struct rtgui_gesture *gest);
    virtual void render(struct rtgui_dc* dc, const Point &dcPoint = Point(), const Rect &srcRect = Rect(), RenderFlag flags = DrawNormal);

    virtual void act(unsigned int num)
    {
        //rt_kprintf(" act num : %d\n", num);
    }

    virtual void end(unsigned int num)
    {
        //rt_kprintf(" end num : %d\n", num);
    }

    virtual void tap(unsigned int num)
    {
        //rt_kprintf(" tap num : %d\n", num);
    }

    void setShowSize(unsigned int normal, unsigned int center = 0)
    {
        normalSize = normal;
        centerSize = normal > center ? normal : center;
    }

    void setShowItems(unsigned int items)
    {
        showItems = (items & (unsigned) - 2) + 1;
    }

    void setItemSize(unsigned int size)
    {
        itemSize = size;
    }

    int getCenterSelNum(void)
    {
        return centerSelNum;
    }

    void setDirection(enum type t)
    {
        direction = t;

        if (direction & HORIZONTAL)
            setInterestGesture(RTGUI_GESTURE_TAP | RTGUI_GESTURE_LONGPRESS | RTGUI_GESTURE_DRAG_HORIZONTAL);
        else
            setInterestGesture(RTGUI_GESTURE_TAP | RTGUI_GESTURE_LONGPRESS | RTGUI_GESTURE_DRAG_VERTICAL);
    }

    void enableBindTap(bool tap = true)
    {
        bindTapEnable = tap;
    }

    void enableTapSwitch(bool tap = true)
    {
        bindTapSwitch = tap;
    }

    void switchNextItem(void)
    {
        switchItem(false);
    }

    void switchPrevItem(void)
    {
        switchItem(true);
    }

    void setInertialSeeds(unsigned int seeds)
    {
        if (seeds > 0)
            inertialSeeds = seeds;
    }

    void setRadius(int radius)
    {
        if (radius >= 0 && radius != m_radius)
        {
            m_radius = radius;
            invalidate();
        }
    }

protected:
    void onAnimation(int progress);
    void animFinished();
    void stopAnimation(void);
    void switchItem(bool type);

    enum type direction;
    int movePitch, gestPitch;
    int centerSelNum;

    int normalSize, centerSize, showItems, itemSize;
    int firstItemPitch, firstItemNum, itemMovePitch;
    int centerNum, oldNum;

    int animMovePitch, animMoveProgress, animProgress;
    unsigned int inertialSeeds;
    bool isFirstRender;
    bool bindTapEnable, bindTapSwitch;
    IntAnimator *m_anim;
    int m_radius;
};

class WheelImage : public Wheel
{
public:
    WheelImage();
    WheelImage(const Rect& rect);
    virtual ~WheelImage();

    void addImage(Image *img);

    void setForeImage(Image *img)
    {
        if (foreImage)
            delete foreImage;

        foreImage = img;
    }

    virtual void act(unsigned int num)
    {
        bindChanged(num);
    }

    virtual void tap(unsigned int num)
    {
        bindTap(num);
    }

    virtual void render(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

    Signal<unsigned int> bindChanged, bindTap;

protected:
    std::vector<Image *> image;

private:
    Image *foreImage;
};

class WheelString : public Wheel
{
public:
    WheelString();
    WheelString(const Rect& rect);
    virtual ~WheelString();

    void addString(const char *str);
    void deleteString(int index);
    void deleteAllString();
    void addCenterString(const char *str);
    void setCenterSuffix(const char* str);
    void setFontColor(rtgui_color_t nor, rtgui_color_t sel)
    {
        norColor = nor;
        selColor = sel;
    }

    void setForeImage(Image *img)
    {
        if (foreImage)
            delete foreImage;

        foreImage = img;
    }

    void setNorFont(rtgui_font_t *norFont)
    {
        this->norFont = norFont;
        invalidate();
    }

    void setSelFont(rtgui_font_t *selFont)
    {
        this->selFont = selFont;
        invalidate();
    }

    void signalEnableString(bool enable = true)
    {
        signalString = enable;
    }

    virtual void act(unsigned int num)
    {
        if (signalString)
            bindChangedS(m_normalList[num]);
        else
            bindChanged(num);
    }

    virtual void tap(unsigned int num)
    {
        if (signalString)
            bindTapS(m_normalList[num]);
        else
            bindTap(num);
    }

    virtual std::string dumpValue() const;

    virtual void render(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

    Signal<unsigned int> bindChanged, bindTap;
    Signal<std::string> bindChangedS, bindTapS;

protected:
    std::vector<std::string> m_normalList, m_centerList;

private:
    void set_draw_clip(int area);

private:
    rtgui_color_t norColor, selColor;
    Image *foreImage;
    bool signalString;
    rtgui_font_t *norFont, *selFont;
};

class CarouselImage : public Wheel
{
public:
    CarouselImage();
    CarouselImage(const Rect& rect);
    virtual ~CarouselImage();

    void addImage(Image *img);
    void addString(const char* str);
    void removeItem(const char* str);
    void emptyItem(void);
    void selectItem(const char* str);

    std::string getItemString(int index)
    {
        std::string none;

        if (index >= 0 && index < (int)this->str.size())
        {
            return this->str[index];
        }

        return none;
    }

    void setSpaceDeep(int spaceDeep)
    {
        this->spaceDeep = spaceDeep;
    }

    void setSpaceX(int spaceX)
    {
        this->spaceX = spaceX;
    }

    void setVisualDistance(int visualDistance)
    {
        this->visualDistance = visualDistance;
    }

    void setVisualHeight(int visualHeight)
    {
        this->visualHeight = visualHeight;
    }

    void setZoom(double zoom)
    {
        this->zoom = zoom;
    }

    void setForeImage(Image *img)
    {
        if (foreImage)
            delete foreImage;

        foreImage = img;
    }

    virtual void act(unsigned int num)
    {
        bindChanged(num);
    }

    virtual void tap(unsigned int num)
    {
        bindTap(num);
    }

    virtual void render(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

    Signal<int> bindChanged, bindTap;

protected:
    std::vector<std::string> str;
    std::vector<Image *> image;

private:
    Image *foreImage;
    double zoom;
    int spaceDeep;
    int spaceX;
    int visualDistance;
    int visualHeight;
};

class DemoWidgetWheel : public Wheel
{
public:
    DemoWidgetWheel(const Rect& rect);
    virtual ~DemoWidgetWheel();

    void addWidget(Widget *widget);

    virtual void render(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

protected:
    std::vector<Widget *> widget;

private:

};

}
