/*
 * File      : pm_version.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-09-05     realthread   the first version
 */
#ifndef __PM_VERSION_H__
#define __PM_VERSION_H__

#include "rtconfig.h"

#ifndef PERSIMMON_MAINVERSION
#define PERSIMMON_MAINVERSION   3
#define PERSIMMON_SUBVERSION    0
#define PERSIMMON_REVISION      0
#endif

#define _STR_(s)                #s
#define __STR(s)                _STR_(s)

#define PERSIMMON_VERSION       __STR(PERSIMMON_MAINVERSION) "." \
                                __STR(PERSIMMON_SUBVERSION) "." \
                                __STR(PERSIMMON_REVISION)

#define PERSIMMON_SOFTWARE_NAME "Persimmon"

#endif
