/*
 * File      : driver.h
 * This file is part of RT-Thread GUI Engine
 * COPYRIGHT (C) 2006 - 2020, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-10-04     Bernard      first version
 */
#ifndef __RTGUI_DRIVER_H__
#define __RTGUI_DRIVER_H__

#include <rtgui/list.h>
#include <rtgui/color.h>

 /**
 * graphic device control command
 */
#define RTGRAPHIC_CTRL_RECT_UPDATE      0
#define RTGRAPHIC_CTRL_POWERON          1
#define RTGRAPHIC_CTRL_POWEROFF         2
#define RTGRAPHIC_CTRL_GET_INFO         3
#define RTGRAPHIC_CTRL_SET_MODE         4
#define RTGRAPHIC_CTRL_GET_EXT          5
#define RTGRAPHIC_CTRL_FRAME_UPDATE     6
#define RTGRAPHIC_CTRL_FRAME_SWITCH     7

 /* graphic deice */
enum
{
    /* pixel format define */
    RTGRAPHIC_PIXEL_FORMAT_MONO = 0,
    RTGRAPHIC_PIXEL_FORMAT_GRAY4,
    RTGRAPHIC_PIXEL_FORMAT_GRAY16,
    RTGRAPHIC_PIXEL_FORMAT_RGB332,
    RTGRAPHIC_PIXEL_FORMAT_RGB444,
    RTGRAPHIC_PIXEL_FORMAT_RGB565,
    RTGRAPHIC_PIXEL_FORMAT_RGB565P,
    RTGRAPHIC_PIXEL_FORMAT_RGB666,
    RTGRAPHIC_PIXEL_FORMAT_RGB888,
    RTGRAPHIC_PIXEL_FORMAT_ARGB888,
    RTGRAPHIC_PIXEL_FORMAT_ALPHA,
    RTGRAPHIC_PIXEL_FORMAT_LUT,
    RTGRAPHIC_PIXEL_FORMAT_COLOR,
    RTGRAPHIC_PIXEL_FORMAT_TSC4,            /* GPU 特定支持的压缩文件格式, apollo4, 压缩比 1:6 */
    RTGRAPHIC_PIXEL_FORMAT_TSC6,            /* GPU 特定支持的压缩文件格式, apollo4, 压缩比 1:4 */
    RTGRAPHIC_PIXEL_FORMAT_TSC6A,           /* GPU 特定支持的压缩文件格式, apollo4, 压缩比 1:4, 带 alpha 通道 */
    /* color format alias */
    RTGRAPHIC_PIXEL_FORMAT_BGR565 = RTGRAPHIC_PIXEL_FORMAT_RGB565P
};

/**
 * build a pixel position according to (x, y) coordinates.
 */
#define RTGRAPHIC_PIXEL_POSITION(x, y)  ((x << 16) | y)

 /**
  * graphic device information structure
  */
struct rt_device_graphic_info
{
    rt_uint8_t  pixel_format;                           /**< graphic format */
    rt_uint8_t  bits_per_pixel;                         /**< bits per pixel */
    rt_uint16_t pitch;                                  /**< bytes per line */

    rt_uint16_t width;                                  /**< width of graphic device */
    rt_uint16_t height;                                 /**< height of graphic device */

    rt_uint8_t *framebuffer;                            /**< frame buffer */
};

/**
 * rectangle information structure
 */
struct rt_device_rect_info
{
    rt_uint16_t x;                                      /**< x coordinate */
    rt_uint16_t y;                                      /**< y coordinate */
    rt_uint16_t width;                                  /**< width */
    rt_uint16_t height;                                 /**< height */
};

/**
 * graphic operations
 */
struct rt_device_graphic_ops
{
    void (*set_pixel) (const char *pixel, int x, int y);
    void (*get_pixel) (char *pixel, int x, int y);

    void (*draw_hline)(const char *pixel, int x1, int x2, int y);
    void (*draw_vline)(const char *pixel, int x, int y1, int y2);

    void (*blit_line) (const char *pixel, int x, int y, rt_size_t size);
};
#define rt_graphix_ops(device)          ((struct rt_device_graphic_ops *)(device->user_data))

/* graphic driver operations */
struct rtgui_graphic_driver_ops
{
    /* set and get pixel in (x, y) */
    void (*set_pixel)(rtgui_color_t *c, int x, int y);
    void (*get_pixel)(rtgui_color_t *c, int x, int y);

    void (*draw_hline)(rtgui_color_t *c, int x1, int x2, int y);
    void (*draw_vline)(rtgui_color_t *c, int x , int y1, int y2);
    void(*fill_rect)(rtgui_color_t *c, int x1, int y1, int x2, int y2);

    /* draw raw hline */
    void (*draw_raw_hline)(rt_uint8_t *pixels, int x1, int x2, int y);
};

/* graphic extension operations */
struct rtgui_graphic_ext_ops
{
    /* some 2D operations */
    void (*draw_line)(rtgui_color_t *c, int x1, int y1, int x2, int y2);

    void (*draw_rect)(rtgui_color_t *c, int x1, int y1, int x2, int y2);
    void (*fill_rect)(rtgui_color_t *c, int x1, int y1, int x2, int y2);

    void (*draw_circle)(rtgui_color_t *c, int x, int y, int r);
    void (*fill_circle)(rtgui_color_t *c, int x, int y, int r);

    void (*draw_ellipse)(rtgui_color_t *c, int x, int y, int rx, int ry);
    void (*fill_ellipse)(rtgui_color_t *c, int x, int y, int rx, int ry);
};

struct rtgui_graphic_driver
{
    /* pixel format and byte per pixel */
    rt_uint8_t pixel_format;
    rt_uint8_t bits_per_pixel;
    rt_uint16_t pitch;

    /* screen width and height */
    rt_uint16_t width;
    rt_uint16_t height;

    /* framebuffer address and ops */
    rt_uint8_t *framebuffer;
    struct rt_device* device;

    const struct rtgui_graphic_driver_ops *ops;
    const struct rtgui_graphic_ext_ops *ext_ops;
};

struct rtgui_graphic_driver *rtgui_graphic_driver_get_default(void);

void rtgui_graphic_driver_get_rect(const struct rtgui_graphic_driver *driver, rtgui_rect_t *rect);
void rtgui_graphic_driver_frame_update(const struct rtgui_graphic_driver *driver, rtgui_rect_t *rect);
void rtgui_graphic_driver_frame_switch(const struct rtgui_graphic_driver *driver);
void rtgui_graphic_driver_screen_update(const struct rtgui_graphic_driver *driver, rtgui_rect_t *rect);
rt_uint8_t *rtgui_graphic_driver_get_framebuffer(const struct rtgui_graphic_driver *driver);

rt_err_t rtgui_graphic_set_device(rt_device_t device);
void rtgui_graphic_driver_set_framebuffer(void *fb);

rt_inline struct rtgui_graphic_driver *rtgui_graphic_get_device()
{
    return rtgui_graphic_driver_get_default();
}

#ifdef RTGUI_USING_HW_CURSOR
/*
 * hardware cursor
 */
enum rtgui_cursor_type
{
    RTGUI_CURSOR_ARROW,
    RTGUI_CURSOR_HAND,
};

void rtgui_cursor_set_device(const char* device_name);
void rtgui_cursor_set_position(rt_uint16_t x, rt_uint16_t y);
void rtgui_cursor_set_image(enum rtgui_cursor_type type);
#endif

#ifdef GUIENGIN_USING_VFRAMEBUFFER
void rtgui_graphic_driver_vmode_enter(void);
void rtgui_graphic_driver_vmode_exit(void);
#endif

struct rtgui_dc* rtgui_graphic_driver_get_rect_buffer(
    const struct rtgui_graphic_driver *driver, struct rtgui_rect *rect);
rt_bool_t rtgui_graphic_driver_is_vmode(void);
void rtgui_send_vsync(void);

#endif

