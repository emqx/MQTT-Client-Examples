/*
 * File      : font_freetype.h
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
 * 2010-04-10     Bernard      first version
 */

#ifndef __RTGUI_FONT_TTF_H__
#define __RTGUI_FONT_TTF_H__

#include <rtgui/dc.h>
#include <rtgui/font.h>
#include <rtgui/filerw.h>

#ifdef __cplusplus
extern "C" {
#endif

void rtgui_font_freetype_system_init(void);
struct rtgui_font *rtgui_font_freetype_create(const char *filename, rt_size_t size, const char *font_family);
void rtgui_font_freetype_reload(const char *filename, const char *font_family);
#ifdef __cplusplus
}
#endif

#endif
