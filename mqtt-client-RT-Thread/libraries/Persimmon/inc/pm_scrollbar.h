/*
 * File      : pm_scrollbar.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */
#ifndef __PM_SCROLLBAR_H
#define __PM_SCROLLBAR_H

#include <pm_widget.h>

namespace Persimmon
{

class IntAnimator;

class AbstractBar : public Widget
{
public:
    AbstractBar();
    virtual ~AbstractBar();

    int value() const { return m_value; }
    int pageStep() const { return m_pageStep; }
    int minimum() const { return m_minimum; }
    int maximum() const { return m_maximum; }
    int range() const { return m_maximum - m_minimum; }

    void setValue(int value);
    void setPageStep(int step);
    void setMinimum(int value);
    void setMaximum(int value);
    void setRange(int minimum, int maximum);
    void setBarBackground(rtgui_color_t color);
    void fadeInOut(bool visiable);

protected:
    rtgui_color_t barForeground() const;
    rtgui_color_t barBackground() const;
    int barAlpha() const { return m_alpha; }

private:
    void setAlpha(int alpha);

private:
    bool m_visiabled;
    int m_value, m_pageStep;
    int m_minimum, m_maximum;
    int m_alpha;
    rtgui_color_t m_barBackground;
    IntAnimator *m_anim;
};

class ScrollBar : public AbstractBar
{
public:
    ScrollBar();
    virtual ~ScrollBar();

    virtual void render(struct rtgui_dc *dc, const Point &dcPoint = Point(),
        const Rect &srcRect = Rect(),
        RenderFlag flags = DrawNormal);

private:
    int m_value;
};

};

#endif
