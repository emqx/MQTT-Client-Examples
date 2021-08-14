/*
 * File      : pm_multi_textbox.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */

#pragma once

#include <pm_widget.h>
#include <vector>
#include <sigslot.h>

namespace Persimmon
{

class MultiTextBox : public Widget
{
public:
    MultiTextBox();
    MultiTextBox(const Rect &rect);
    MultiTextBox(struct rtgui_font *font, const Rect &rect);
    MultiTextBox(const MultiTextBox &other);
    virtual ~MultiTextBox();

    virtual Widget *depthCopy(void) const;
    void addText(const char *text);
    void addText(char text);
    void addText(int num);

    void setLineAlign(int align);
    char *getText(void);
    int getCurrentLines(void);
    int getTotalHeight(void);
    char *getLineText(int index);
    void cutText(void);
    void clearText(void);
    void setRowHeight(int h);
    void updateHeight(void);
    void automaticHeight(bool automatic = true);
    void branch(void);

    virtual std::string dumpValue() const
    {
        return text ? std::string(text) : std::string();
    }

    virtual void render(struct rtgui_dc *dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

    Signal<int> clicked;

private:
    void updateText(const char *fmt, ...);
    std::vector<char *> textLinesPtr;

    char *text;
    int rowHeight, currentLine;
    int textTotalLen;
    bool automaticH;
    int sizeHeight, lineAlign;  /* lineAlign : RTGUI_ALIGN_TOP  RTGUI_ALIGN_CENTER  RTGUI_ALIGN_BOTTOM */
};

} // namespace Persimmon
