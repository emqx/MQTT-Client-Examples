/*
 * File      : pm_barcode.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-09-05     realthread   the first version
 */
#ifndef __PM_BARCODE_H
#define __PM_BARCODE_H

#include <pm_widget.h>

namespace Persimmon
{

class BarCode : public Widget
{
public:
	BarCode();
    virtual ~BarCode();

	void setBarCode(const char *code, size_t size);

    virtual void render(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

private:
	void setColumn(int column, int value, int size);

private:
    struct rtgui_dc *m_codeDc;
};

}

#endif
