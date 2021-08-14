/*
 * File      : pm_textbox.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */

#pragma once

#include <pm_widget.h>

namespace Persimmon
{

class TextBox : public Widget
{
public:
    TextBox(const Rect& rect);
    TextBox(struct rtgui_font* font, const Rect& rect);
    virtual ~TextBox();

    void addText(char* text);
    void addText(char text);
    void addText(int num);
    void cutText(void);

    char* getText(void);
    void clearText(void);
    void setTextMaxLen(int len)
    {
        textMaxLen = len;
    }

    void enableMask(bool mas = true)
    {
        mask = mas;
    }

    virtual std::string dumpValue() const { return text ? std::string(text) : std::string(); }

    virtual void render(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

private:
    void updateText(const char* fmt, ...);

    char *text;
    int textMaxLen, textLen;
    bool mask;
};

}
