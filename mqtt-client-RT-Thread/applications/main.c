/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-09-02     RT-Thread    first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "drv_common.h"
#include "mqtt_client.h"


#define LED_PIN GET_PIN(I, 8)

#define WIFI_SSID "LINKSYS_EMQX_IOT"
#define WIFI_PASSWORD "emqx1025"

extern void wlan_autoconnect_init(void);

int main(void)
{
    rt_pin_mode(LED_PIN, PIN_MODE_OUTPUT);

    rt_wlan_connect(WIFI_SSID, WIFI_PASSWORD);

    /* init Wi-Fi auto connect feature */
    wlan_autoconnect_init();
    /* enable auto reconnect on WLAN device */
    rt_wlan_config_autoreconnect(RT_TRUE);

    rt_thread_mdelay(10000);

    mqtt_client_start();

    for (;;)
    {
        rt_thread_mdelay(500);
        rt_pin_write(LED_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED_PIN, PIN_LOW);
    }

    mqtt_client_stop();

    return RT_EOK;
}

#include "stm32h7xx.h"
static int vtor_config(void)
{
    /* Vector Table Relocation in Internal QSPI_FLASH */
    SCB->VTOR = QSPI_BASE;
    return 0;
}
INIT_BOARD_EXPORT(vtor_config);

