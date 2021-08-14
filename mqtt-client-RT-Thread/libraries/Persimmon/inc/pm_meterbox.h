/*
 * File      : pm_meterbox.h
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

class MeterBox : public Widget
{
public:
    MeterBox(Image* img, int length);
    virtual ~MeterBox();

    Signal<void> finished;

    void setCenter(int x, int y);
    void setDegreeRange(double min, double max);
    void setValueRange(double min, double max);
    void setTimes(int times);
    void setValue(double value);

    virtual void render(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

private:
    void timeout(void);

protected:
    struct rtgui_dc *pointer_dc;
    struct rtgui_point p_center, b_center;
    Timer *timer;
    int times;
    double min_degree, max_degree, min_value, max_value, pointer_degree, rota_degree, once_degree;
};

}
