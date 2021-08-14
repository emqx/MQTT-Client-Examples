/*
 * File      : pm_imagerotobox.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */

#pragma once

#include <pm_widget.h>
#include <pm_timer.h>
#include <sigslot.h>

namespace Persimmon
{

class ImageRotoBox : public Widget
{
public:
    ImageRotoBox(const char* img_name);
    virtual ~ImageRotoBox();

    void start(int once_ticks, int frame, bool once = true);
    void stop(void);

    Signal<void> finished;

    virtual void render(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

private:
    void timeout(void);

protected:
    struct rtgui_dc *img_dc;
    Timer *timer;
    int coord_x, coord_y, frame;
    float roto_degree;
    bool roto_mode;
};

}
