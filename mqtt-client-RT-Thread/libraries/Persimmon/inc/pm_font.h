/*
 * File      : pm_font.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */

#pragma once

#include <rtgui/rtgui.h>
#include <rtgui/font.h>
#include <pm_rect.h>

#ifdef RTGUI_USING_TTF
#include <rtgui/font_freetype.h>
#endif

namespace Persimmon
{

/**
 * Font class
 */
class Font
{
public:
    Font(struct rtgui_font* font);
    ~Font();

    Rect getMetrics(const char* text);
    void getMetrics(const char* text, struct rtgui_rect *rect);
    int getWidth(const char* text);
    int getHeight(const char* text);

    void setFont(struct rtgui_font *font)
    {
        this->font = font;
    }

    struct rtgui_font* getFont()
    {
        return font;
    }

private:
    struct rtgui_font* font;
};

/*
 * FreeType Font
 */
class FTFont : public Font
{
public:
    FTFont(const char* fontname, int size);
    ~FTFont();

private:
};

}
