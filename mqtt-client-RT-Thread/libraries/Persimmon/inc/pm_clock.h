/*
 * File      : pm_clock.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-29     realthread   the first version
 */

#pragma once

#include <pm_widget.h>
#include <pm_image.h>
#include <pm_container.h>
#include <pm_transform.h>
#include <pm_pixmap.h>

#ifdef USE_GPU
#include "nema_core.h"
#endif

namespace Persimmon
{

class FloatAnimator;

class Clock : public Container
{
public:
    Clock();
    virtual ~Clock();

    void setCenterImg(Image *img)
    {
        if (center)
            delete center;

        center = img;
        invalidate();
    }

    void enablePointerCache(bool enabled);

    void setTime(int time);

    void setHourImg(Image *img);
    void setMinuteImg(Image *img);
    void setSecondImg(Image *img);

    void setHourAnchor(const PointF &anchor);
    void setMinuteAnchor(const PointF &anchor);
    void setSecondAnchor(const PointF &anchor);

    void setSmoothAnim(bool enabled, int interval = 50);
    void enableMillisecond(bool enabled);

    virtual void render(struct rtgui_dc *dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

private:
    struct PointerBlitInfo
    {
        struct rtgui_dc *dcbuf;
        rtgui_rect_t rect;
    };
    void setTransAngle(float angle);
    void updateView();
    Pixmap updatePointer(const Pixmap &pixmap, float degree, const PointF &anchor);
    void updatePointers(bool force = false);
    void drawPointer(struct rtgui_dc *dc, const Pixmap &pixmap, const Point &position);

#ifdef USE_GPU
    struct quad_t
    {
        float x0, y0;
        float x1, y1;
        float x2, y2;
        float x3, y3;
    };
    enum needle_t 
    {
        SECOND_NEEDLE = 0,
        MINUTE_NEEDLE = 1,
        HOUR_NEEDLE = 2,
        MAX_NEEDLE
    };
    void gpu_updateNeedle(void);
    void gpu_updateSecond(enum needle_t needle);
    void gpu_updateCenter();
#endif

private:
    Image *center;
    Pixmap m_hourPixmap, m_minutePixmap, m_secondPixmap;
    PointF m_hourAnchor, m_minuteAnchor, m_secondAnchor;
    Pixmap m_transformedPixmap[3];
    int timeSec, m_lastSec, m_msec, m_lastMsec;
    bool m_useSmoothAnim, m_cacheEnabled, m_useMsec;
    float m_transAngle;
    FloatAnimator *m_anim;

#ifdef USE_GPU
    nema_cmdlist_t gpu_cl;
    float m_hourdegree, m_minutedegree, m_seconddegree;
#endif

};

}
