/*
 * File      : pm_label.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date             Author          Notes
 * 2017-11-05       realthread      the first version
 */

#pragma once

#include <pm_widget.h>
#include <pm_timer.h>

namespace Persimmon
{

class Label : public Widget
{
public:
    Label();
#if 0
    Label(const char *label);
    Label(const char *label, const Rect &rect);
    Label(const char *label, struct rtgui_font *font);
    Label(const char *label, struct rtgui_font *font, const Rect &rect);
    Label(const Label &other);
#endif
    virtual ~Label();
#if 0
    virtual Widget *depthCopy(void) const;
#endif
    void setLabel(const char *label);
    void setLabel(int number);
    void setLabel(char *fmt, ...);

    char *getLabel(void) const;
    void clearLabel(void);

    void enableScroll(bool scroll = true);
    void setScroll(int pitch, unsigned int ms);

    void enableAdapt(bool adapt = true);
    void setAdaptAlign(int align);
    void setBindWidget(Widget *bind);
    void setBindWidgetAlign(int bindWidgetAlign);

    void setEllipsis(const char *ellipsis);
    void branch(void);

    virtual void render(struct rtgui_dc *dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

    void setSuffix(const char *text);
    void setSuffixFont(struct rtgui_font *font);

    virtual std::string dumpValue() const
    {
        return label ? std::string(label) : std::string();
    }

private:
    void checkScroll(void);
    void timeout(void);

    void checkAdapt(void);

    char *label, *suffix;
    struct rtgui_font *suffixFont;

    bool scroll;
    Timer *timer;
    int scrollX, textW, scrollPitch, scrollInterval;
    rt_uint16_t textalign;

    bool adapt;
    int adaptAlign;
    Widget *bindW;
    int bindAlign;
    char *ellipsis;
};

} // namespace Persimmon
