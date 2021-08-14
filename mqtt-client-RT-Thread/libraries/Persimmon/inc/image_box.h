/*
 * File      : image_box.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */

#pragma once

#include <pm_widget.h>
#include <pm_image.h>

namespace Persimmon
{

class ImageBox : public Widget
{
public:
    ImageBox();
    ImageBox(Image *image);
    virtual ~ImageBox();

    void setImage(Image *image);
    void setProvisionalImage(Image *image);
    Image* getImage(void);

    virtual void render(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

protected:
    Image *image, *provisionalImg;
};

}
