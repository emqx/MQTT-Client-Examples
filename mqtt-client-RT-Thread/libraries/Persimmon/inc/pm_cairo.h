/*
 * File      : pm_cairo.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-09     realthread   the first version
 */

#pragma once

#include <pm_widget.h>
#include <vector>

#ifdef PKG_USING_CAIRO

#include <cairo.h>

#include <rtgui/font.h>
#include <rtgui/font_fnt.h>

#ifdef GUIENGINE_USING_TTF
#include <rtgui/font_freetype.h>
#endif

#include <resource.h>

#define CAIRO_LAYERS  5

namespace Persimmon
{

class Cairo : public Widget
{
public:
    Cairo();
    virtual ~Cairo();

    bool createCanvasContext(int layer = 0);
    void canvasContextDestroy(int layer);
    void setFillStyle(rtgui_color_t color);
    void setFillStyle(cairo_pattern_t *source);
    void setStrokeStyle(rtgui_color_t color);
    void setStrokeStyle(cairo_pattern_t *source);
    void setLineWidth(double width);
    void setLineCap(cairo_line_cap_t line_cap);
    void setLineJoin(cairo_line_join_t line_join);
    void setLineDash(const double *dashes, int num_dashes, double offset);
    void setMiterLimit(double limit);
    void rect(double x, double y, double width, double height);
    void fillRect(double x, double y, double width, double height);
    void strokeRect(double x, double y, double width, double height);
    void clearRect(double x, double y, double width, double height);
    void fill(void);
    void stroke(void);
    void beginPath(void);
    void closePath(void);
    void moveTo(double x, double y);
    void lineTo(double x, double y);
    void arc(double xc, double yc, double radius, double angle1, double angle2);
    void bezierCurveTo(double x1, double y1, double x2, double y2, double x3, double y3);
    void scale(double sx, double sy);
    void rotate(double angle);
    void translate(double tx, double ty);
    void clip(void);
    void resetClip(void);
    void setGlobalAlpha(double alpha);
    void save(void);
    void restore(void);
    void drawImageSimple(Image *img, double dx, double dy, double dWidth, double dHeight);
    void drawImage(Image *img, double dx, double dy, double dWidth, double dHeight, double sx, double sy, double sWidth, double sHeight);
    void setCairoFont(const char *family, double size);
    void fillText(const char *utf8, double x, double y);
    void setTextAlign(enum RTGUI_ALIGN type);
    void setTextBaseline(enum RTGUI_ALIGN type);
    void draw(void);
    void saveFontFamily(const char *family);
    char *getFontFamily(void);
    cairo_pattern_t * createLinearGradient(double x0, double y0, double x1, double y1);
    cairo_pattern_t * createCircularGradient(double cx0, double cy0, double radius0, double cx1, double cy1, double radius1);
    void addColorStop(cairo_pattern_t *pattern, double offset, double red, double green, double blue, double alpha);
    virtual void render(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

protected:

private:
    void cairoCreate(int layer);
    void setSource(rtgui_color_t color);
    void setSource(cairo_pattern_t *source);

    cairo_t *cr;
    cairo_surface_t *surface;
    struct rtgui_dc *cairoDc, *layerDc[CAIRO_LAYERS];
    std::vector<cairo_pattern_t *> cairo_pattern;
    rtgui_color_t fillStyle, strokeStyle;
    bool isSetFillStyle, isSetStrokeStyle;
    cairo_pattern_t *patternFillStyle, *patternStrokeStyle;
    rtgui_color_t saveFillStyle, saveStrokeStyle;
    cairo_pattern_t *savePatternFillStyle, *savePatternStrokeStyle;
    double globalAlpha;
    enum RTGUI_ALIGN textAlign, textBaseline;
    char *fontFamily;
};

}

#endif
