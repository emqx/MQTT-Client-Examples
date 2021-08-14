/*
 * File      : pm_canvas.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */

#pragma once

#include <pm_widget.h>
#include <vector>

namespace Persimmon
{

class Canvas : public Widget
{
public:
    enum drawType
    {
        RECT = 1,
        FILL_STYLE,
        FILL,
        FILL_RECT,
        STROKE_STYLE,
        STROKE,
        STROKE_RECT,
        CLEAR_RECT,
        B_PATH,
        C_PATH,
        MOVE_TO,
        LINE_TO,
        LINE_WIDTH,
        LINE_CAP,
        LINE_JOIN,
        LINE_DASH,
        GLOBAl_ALPHA,
    };

    enum cap
    {
        CAP_NONE = 0,
        CAP_BUTT,
        CAP_ROUND,
        CAP_SQUARE,
    };

    Canvas();
    virtual ~Canvas();

    void setShadow(int offsetX, int offsetY, int blur, rtgui_color_t color)
    {

    }

    void rect(rt_int16_t x, rt_int16_t y, rt_int16_t width, rt_int16_t height)
    {
        rtgui_rect_t rect;
        
        rect.x1 = x;
        rect.y1 = y;
        rect.x2 = x + width;
        rect.y2 = y + height;

        _type.push_back(RECT);
        _index.push_back(_rect.size());
        _rect.push_back(rect);
    }

    void setFillStyle(rtgui_color_t color)
    {
        _type.push_back(FILL_STYLE);
        _index.push_back(_color.size());
        _color.push_back(color);
    }

    void fill(void)
    {
        _index.push_back(_type.size());
        _type.push_back(FILL);
    }

    void fillRect(rt_int16_t x, rt_int16_t y, rt_int16_t width, rt_int16_t height)
    {
        rtgui_rect_t rect;
        
        rect.x1 = x;
        rect.y1 = y;
        rect.x2 = x + width;
        rect.y2 = y + height;

        _type.push_back(FILL_RECT);
        _index.push_back(_rect.size());
        _rect.push_back(rect);
    }

    void setStrokeStyle(rtgui_color_t color)
    {
        _type.push_back(STROKE_STYLE);
        _index.push_back(_color.size());
        _color.push_back(color);
    }

    void stroke(void)
    {
        _index.push_back(_type.size());
        _type.push_back(STROKE);
    }

    void strokeRect(rt_int16_t x, rt_int16_t y, rt_int16_t width, rt_int16_t height)
    {
        rtgui_rect_t rect;
        
        rect.x1 = x;
        rect.y1 = y;
        rect.x2 = x + width;
        rect.y2 = y + height;

        _type.push_back(STROKE_RECT);
        _index.push_back(_rect.size());
        _rect.push_back(rect);
    }

    void clearRect(rt_int16_t x, rt_int16_t y, rt_int16_t width, rt_int16_t height)
    {
        rtgui_rect_t rect;
        
        rect.x1 = x;
        rect.y1 = y;
        rect.x2 = x + width;
        rect.y2 = y + height;

        _type.push_back(CLEAR_RECT);
        _index.push_back(_rect.size());
        _rect.push_back(rect);
    }

    void moveTo(rt_int16_t x, rt_int16_t y)
    {
        rtgui_point_t point = { x, y };

        _type.push_back(MOVE_TO);
        _index.push_back(_point.size());
        _point.push_back(point);
    }

    void lineTo(rt_int16_t x, rt_int16_t y)
    {
        rtgui_point_t point = { x, y };

        _type.push_back(LINE_TO);
        _index.push_back(_point.size());
        _point.push_back(point);
    }

    void setLineWidth(unsigned int width)
    {
        _type.push_back(LINE_WIDTH);
        _index.push_back(_lineWidth.size());
        _lineWidth.push_back(width);
    }

    void setLineCap(std::string &str)
    {
        _type.push_back(LINE_CAP);
        _index.push_back(_string.size());
        _string.push_back(str);
    }

    void setLineJoin(std::string &str)
    {
        _type.push_back(LINE_JOIN);
        _index.push_back(_string.size());
        _string.push_back(str);
    }

    void setLineDash(std::vector<unsigned int> pattern, unsigned int offset)
    {
        _type.push_back(LINE_JOIN);
        //_index.push_back(_string.size());
        //_string.push_back(str);
    }

    void beginPath(void)
    {
        _type.push_back(B_PATH);
        _index.push_back(_type.size());
    }

    void closePath(void)
    {
        _type.push_back(C_PATH);
        _index.push_back(_type.size());
    }

    void setGlobalAlpha(rt_uint8_t a)
    {
        _type.push_back(GLOBAl_ALPHA);
        _index.push_back(global_alpha.size());
        global_alpha.push_back(a);
    }

    rtgui_color_t getFillStyleC(void)
    {
        return RTGUI_ARGB(RTGUI_RGB_A(fillStyleC) * globalAlpha / 0xFF, RTGUI_RGB_R(fillStyleC), RTGUI_RGB_G(fillStyleC), RTGUI_RGB_B(fillStyleC));
    }

    rtgui_color_t getStrokeStyleC(void)
    {
        return RTGUI_ARGB(RTGUI_RGB_A(strokeStyleC) * globalAlpha / 0xFF, RTGUI_RGB_R(strokeStyleC), RTGUI_RGB_G(strokeStyleC), RTGUI_RGB_B(strokeStyleC));
    }

    void clearActions(void)
    {
        beginPathIndex = lineWidth = 0;
        lineCap = CAP_NONE;
        startPoint = rtgui_empty_point;
        lastPoint = rtgui_empty_point;
        fillStyleC = strokeStyleC = BLACK;
        globalAlpha = 0xFF;

        _type.clear();
        _index.clear();
        _lineWidth.clear();
        _rect.clear();
        _color.clear();
        _point.clear();
        global_alpha.clear();
        _string.clear();
    }

    void draw(struct rtgui_dc* dc, int index);

    virtual void render(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

protected:
    std::vector<enum drawType> _type;
    std::vector<unsigned int> _index;
    std::vector<unsigned int> _lineWidth;
    std::vector<rtgui_rect_t> _rect;
    std::vector<rtgui_color_t> _color;
    std::vector<rtgui_point_t> _point;
    std::vector<rt_uint8_t> global_alpha;
    std::vector<std::string> _string;

private:
    unsigned int beginPathIndex, lineWidth, lineCap;
    rtgui_point_t startPoint, lastPoint;
    rtgui_color_t fillStyleC, strokeStyleC;
    rt_uint8_t globalAlpha;
};

}
