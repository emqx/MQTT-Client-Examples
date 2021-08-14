/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-08-02     liuduanfei   first version
 */

#ifndef _WM_8988_H_
#define _WM_8988_H_

#include <rtdevice.h>

void wm8988_init(struct rt_i2c_bus_device *dev);
void wm8988_set_out_valume(struct rt_i2c_bus_device *dev, rt_uint8_t val);

#endif /* _WM_8988_H_ */
