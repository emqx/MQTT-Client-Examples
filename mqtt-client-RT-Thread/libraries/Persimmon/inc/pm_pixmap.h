/*
 * File      : pm_pixmap.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */
#ifndef __PM_PIXMAP_H
#define __PM_PIXMAP_H

#include <rtgui/dc.h>
#include <rtgui/driver.h>
#include <pm_rect.h>

namespace Persimmon
{

class Transform;

class Pixmap
{
public:
    enum TransformationMode
    {
        FastTransformation,
        SmoothTransformation
    };

    Pixmap();
    Pixmap(int width, int height, int xPos = 0, int yPos = 0, int pixfmt = RTGRAPHIC_PIXEL_FORMAT_ARGB888);
    Pixmap(rtgui_dc *dc, bool newBuffer = true, int xPos = 0, int yPos = 0);
    Pixmap(const Pixmap &other);
    ~Pixmap();

    rt_bool_t isEmpty() const
    {
        return ((m_dcbuf == RT_NULL) ? RT_TRUE : RT_FALSE);
    }
    Rect rect() const;
    Size size() const;
    int xPos() const
    {
        return m_xPos;
    }
    int yPos() const
    {
        return m_yPos;
    }
    int width() const;
    int height() const;
    void resize(int width, int height, rt_uint8_t pixel_format = RTGRAPHIC_PIXEL_FORMAT_ARGB888);
    void clear();
    rtgui_dc* todc();
    rtgui_dc* constdc() const;
    Pixmap transform(const Transform &tr, TransformationMode mode = FastTransformation) const;
    Pixmap scale(float sx, float sy, TransformationMode mode = FastTransformation) const;
    Pixmap rotate(float angle, TransformationMode mode = FastTransformation) const;

    Pixmap& operator=(const Pixmap &other);

private:
    typedef void (*TransFunc)(const Transform&, int, int, int, int, int, int, rt_uint8_t*, const rt_uint8_t*);
    Rect transformRect(const Transform &tr) const;
    rtgui_dc* transform(const Transform &tr, int *xPos, int *yPos, TransFunc transFunc) const;

private:
    int m_xPos, m_yPos;
    rtgui_dc_buffer *m_dcbuf;
};

};

#endif
