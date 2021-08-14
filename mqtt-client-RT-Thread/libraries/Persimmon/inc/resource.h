/*
 * File      : resource.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */

#pragma once

#include <rtgui/rtgui.h>
#include <pm_image.h>

using namespace Persimmon;

const char *setPrefix(const char *p);
const char *getPrefix(void);
Image* getImage(const char* fmt, ...);
struct rtgui_dc *getImageDc(const char* fmt, ...);
