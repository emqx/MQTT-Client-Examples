/*
 * File      : pm_arcbar.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-06-01     realthread   the first version
 */
#ifndef __PM_ARCBAR_H
#define __PM_ARCBAR_H

#include <pm_scrollbar.h>
#ifdef PKG_USING_VGL
struct vgl_t;

namespace Persimmon
{

class ArcBar : public AbstractBar
{
public:
    ArcBar();
    virtual ~ArcBar();

    void setShape(float cx, float cy, float radius,
                  float angleStart = -10, float angleEnd = 10, float lineWidth = 10);
    void setCenterX(float cx);
    void setCenterY(float cy);
    void setRadius(float radius);
    void setAngleStart(float angle);
    void setAngleEnd(float angle);
    void setAngle(float angleStart, float angleEnd);
    void setLineWidth(float lineWidth);

    virtual void render(struct rtgui_dc *dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

private:
    void drawBar(vgl_t *vgl);
    void drawBgBar(vgl_t *vgl);
    void updateBackground();

private:
    float m_centerX, m_centerY, m_radius;
    float m_angleStart, m_angleEnd, m_lineWidth;
};

class ArcDotIndicator : public AbstractBar
{
public:
    enum DotStyle
    {
        MoveDot,
        LightDot
    };
    ArcDotIndicator();
    virtual ~ArcDotIndicator();

    void setDotStyle(DotStyle dotStyle);
    void setShape(float cx, float cy, float radius,
                  float angleStart = -10, float angleEnd = 10,
                  float dotSizeFocus = 8, float dotSizeNormal = 8);
    void setCenterX(float cx);
    void setCenterY(float cy);
    void setRadius(float radius);
    void setAngleStart(float angle);
    void setAngleEnd(float angle);
    void setAngle(float angleStart, float angleEnd);
    void setDotSizeFocus(float size);
    void setDotSizeNormal(float size);
    void setKeepSpacing(bool enabled);

    virtual void render(struct rtgui_dc *dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

private:
    void drawDots(vgl_t *vgl);
    void drawMoveDot(vgl_t *vgl);
    void drawLightDot(vgl_t *vgl);
    void updateAngleSpacing();
    void updateAngleRange();

private:
    DotStyle m_dotStyle;
    float m_centerX, m_centerY, m_radius;
    float m_angleStart, m_angleEnd;
    float m_dotSizeFocus, m_dotSizeNormal;
    float m_angleSpacing;
};

};

#endif
#endif
