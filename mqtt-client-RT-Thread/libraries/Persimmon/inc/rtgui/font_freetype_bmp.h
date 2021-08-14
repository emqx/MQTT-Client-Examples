/*
 * File      : font_freetype_bmp.h
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
 * 2020-03-15     realthread   first version
 */
#ifndef __FONT_TTF_BMP_H__
#define __FONT_TTF_BMP_H__

#include <rtgui/font.h>
#include <rtgui/filerw.h>

#ifdef GUIENGINE_USING_TTFB

//#define RT_USING_DFS_ROMFS  /* test on simulator */
//#define TTFB_CACHE_ENABLE /* test */

#ifdef TTFB_CACHE_ENABLE
struct element_struct
{
    uint64_t   id;
    void*      data;
};

struct hash_bucket_struct
{
    rt_list_t             hash;
    rt_list_t             queue;
    struct element_struct h_data;
};

struct hash_bucket_handle
{
    rt_list_t           list_head;    /* hash_bucket_struct */
    struct hash_key     *key_buf;
    uint32_t            obj_max_number;
    struct hash_bucket_struct *mem_buff;
};
typedef struct hash_bucket_handle* hb_handle_t;

struct hash_key
{
    rt_list_t head_list;
};
#endif

#define MAGIC   (0x12345678)

struct font_head
{
    char magic[4];
    rt_uint32_t font_cnt;
};

struct code_head
{
    rt_uint32_t size;      // font size
    rt_uint32_t num;       // Number of fonts
    rt_uint32_t pos;       // Initial offset
    rt_uint32_t len;       // Total length
    rt_uint32_t qit_cnt;   // Quick index table count
};

struct font_data
{
    rt_uint16_t width;
    rt_uint16_t height;
    rt_uint16_t left;
    rt_uint16_t top;
    rt_uint32_t xadvance;
    rt_uint32_t yadvance;
    rt_uint32_t uidx;           // code unicode value
    rt_uint8_t buffer[];
};

struct font_data_bmp
{
    rt_uint8_t   width;
    rt_uint8_t   height;
    rt_uint8_t   left;
    rt_uint8_t   top;

    rt_uint8_t   format;
    rt_uint8_t   max_grays;
    rt_uint16_t  pitch;
    rt_uint8_t   xadvance;
    rt_uint8_t   yadvance;

    const rt_uint8_t*  buffer;
};

/* Quick index table */
struct font_qit
{
    rt_uint32_t start;         // Font start number
    rt_uint32_t end;           // Font end number
    rt_uint32_t pit_pos;       // Absolute offset address of precise index table
    rt_uint32_t pit_len;       // Index table size
    rt_uint32_t bitmap_pos;    //
    rt_uint32_t bitmap_size;   //
};

/* Precise index table */
struct font_pit
{
    rt_uint32_t pos;
    rt_uint32_t len;
};

/* Bitmap tab */
struct font_bitmap
{
    rt_uint8_t* bitmap;
    rt_uint32_t len;
};

struct code_obj
{
    struct code_head head;
    struct font_qit *qit;
    struct font_bitmap *bitmap;
};

struct font_obj
{
#ifndef RT_USING_DFS_ROMFS
    struct rtgui_filerw *fp;
#else
    uint8_t *fp;
#endif
    unsigned int code_num;
#ifdef TTFB_CACHE_ENABLE
    struct hash_bucket_handle* handle;
#endif
    struct code_obj *last_code;
    struct code_obj code[];
};

#define font_family_len 64

struct font_ftc_bmp
{
    /* the bmp font list */
    rtgui_list_t    list;
    rt_uint32_t     refer_count;
    char            font_family[font_family_len];

    struct font_obj *font_bmp;
};

void rtgui_font_ftc_bmp_system_init(void);
struct font_ftc_bmp *rtgui_font_ftc_bmp_refer(const char *family);
void rtgui_font_ftc_bmp_derefer(struct font_ftc_bmp *ftc_bmp);
struct font_ftc_bmp *rtgui_font_ftc_bmp_load(const char *filename, const char *font_family);
struct rtgui_font *rtgui_font_ftc_bmp_create(const char *filename, rt_size_t size, const char *font_family);

#endif

#endif
