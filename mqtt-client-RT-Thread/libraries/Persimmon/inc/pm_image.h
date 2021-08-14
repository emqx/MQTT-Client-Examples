/*
 * File      : pm_image.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */

#pragma once

#include <rtgui/image.h>
#include <rtgui/image_container.h>
#include <pm_rect.h>

namespace Persimmon
{

class Image
{
public:
    Image();
    Image(const char *filename, rt_bool_t load = RT_TRUE);
    Image(const Image &other);
    Image(struct rtgui_image* image) { this->image = image; item = NULL; }
    virtual ~Image();

    virtual Image *depthCopy(void) const;

    Rect getRect() const;
    int getWidth() const;
    int getHeight() const;

    struct rtgui_image *getImage(void);
    struct rtgui_image_item *getItem(void);
    void loadImage(const char *filename);
    void loadPNG(const char *filename);

    virtual void render(struct rtgui_dc *dc, const Point &point);
    virtual void render(struct rtgui_dc *dc, const Rect &rect);

protected:
    struct rtgui_image *image;
#ifdef GUIENGINE_IMAGE_CONTAINER
    struct rtgui_image_item *item;
#endif
};

} // namespace Persimmon
