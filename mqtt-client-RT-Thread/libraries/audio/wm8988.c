/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-08-02     liuduanfei   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <wm8988.h>

#define DBG_TAG "wm8988"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define  LEFT_INPUT_VOLUME     (0x00 << 9)  /* R0  */
#define  RIGHT_INPUT_VOLUME    (0x01 << 9)  /* R1  */
#define  LOUT1_VOLUME          (0x02 << 9)  /* R2  */
#define  ROUT1_VOLUME          (0x03 << 9)  /* R3  */
                                            /* R4  */
#define  ADC_DAC_CONTROL       (0x05 << 9)  /* R5  */
                                            /* R6  */
#define  AUDIO_INTERFACE       (0x07 << 9)  /* R7  */
#define  SAMPLE_RATE           (0x08 << 9)  /* R8  */
                                            /* R9  */
#define  LEFT_DAC_VOLUME       (0x0A << 9)  /* R10 */
#define  RIGHT_DAC_VOLUME      (0x0B << 9)  /* R11 */
#define  BASS_CONTROL          (0x0C << 9)  /* R12 */
#define  TREBLE_CONTROL        (0x0D << 9)  /* R13 */
#define  RESET                 (0x0F << 9)  /* R15 */
#define  THREED_CONTROL        (0x10 << 9)  /* R16 */
#define  ALC1                  (0x11 << 9)  /* R17 */
#define  ALC2                  (0x12 << 9)  /* R18 */
#define  ALC3                  (0x13 << 9)  /* R19 */
#define  NOISE_GATE            (0x14 << 9)  /* R20 */
#define  LEFT_ADC_VOLUME       (0x15 << 9)  /* R21 */
#define  RIGHT_ADC_VOLUME      (0x16 << 9)  /* R22 */
#define  ADDITIONAL_CONTROL_1  (0x17 << 9)  /* R23 */
#define  ADDITIONAL_CONTROL_2  (0x18 << 9)  /* R24 */
#define  PWR_MGMT_1            (0x19 << 9)  /* R25 */
#define  PWR_MGMT_2            (0x1A << 9)  /* R26 */
#define  ADDITIONAL_CONTROL    (0x1B << 9)  /* R27 */
#define  ADC_INPUT_MODE        (0x1F << 9)  /* R31 */
#define  ADCL_SIGNAL_PATH      (0x20 << 9)  /* R32 */
#define  ADCR_SIGNAL_PATH      (0x21 << 9)  /* R33 */
#define  LEFT_OUT_MIX_1        (0x22 << 9)  /* R34 */
#define  LEFT_OUT_MIX_2        (0x23 << 9)  /* R35 */
#define  RIGHT_OUT_MIX_1       (0x24 << 9)  /* R36 */
#define  RIGHT_OUT_MIX_2       (0x25 << 9)  /* R37 */
                                            /* R38 */
                                            /* R39 */
#define  LOUT2_VOLUME          (0x28 << 9)  /* R40 */
#define  ROUT2_VOLUME          (0x29 << 9)  /* R41 */
                                            /* R42 */
#define  LOW_POWER_PLAYBACK    (0x43 << 9)  /* R67 */

/* R10 */
#define LDVU      (1<<8)
#define LDACVOL

/* R11 */
#define RDVU      (1<<8)
#define RDACVOL


static rt_uint16_t wm8988_regval_tbl[68] =
{
    0x0097, 0x0097, 0x0079, 0x0079, 0x0000, 0x0008, 0x0000, 0x000A,   /* 00-07 */
    0x0000, 0x0000, 0x00FF, 0x00FF, 0x000F, 0x000F, 0X0000, 0X0000,   /* 08-15 */
    0x0000, 0x007B, 0X012C, 0X002C, 0X002C, 0X002C, 0X002C, 0X0000,   /* 16-23 */
    0X0032, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000,   /* 24-31 */
    0X0038, 0X000B, 0X0032, 0X0000, 0X0008, 0X000C, 0X0093, 0X00E9,   /* 32-39 */
    0X0000, 0X0000, 0X0000, 0X0000, 0X0003, 0X0010, 0X0010, 0X0100,   /* 40-47 */
    0X0100, 0X0002, 0X0001, 0X0001, 0X0039, 0X0039, 0X0039, 0X0039,   /* 48-55 */
    0X0100, 0X0002, 0X0001, 0X0001, 0X0039, 0X0039, 0X0039, 0X0039,   /* 56-63 */
    0X0100, 0X0002, 0X0001, 0X0001,                                   /* 64-67 */
};

#define wm8988_addr (0x1A)

static void wm8988_reg_wrtie(struct rt_i2c_bus_device *dev, rt_uint16_t data)
{
    rt_uint8_t buf[2];

    wm8988_regval_tbl[data >> 9] = data & 0x1FF;

    buf[0] = (rt_uint8_t)(data >> 8);
    buf[1] = (rt_uint8_t)(data);

    struct rt_i2c_msg msg;

    msg.addr  = wm8988_addr;
    msg.buf   = buf;
    msg.flags = RT_I2C_WR;
    msg.len   = 2;

    if (rt_i2c_transfer(dev, &msg, 1) != 1)
    {
        LOG_D("wm8988 write register failed.");
    }
}

static rt_uint16_t wm8988_reg_read(struct rt_i2c_bus_device *dev, rt_uint16_t reg)
{
    return wm8988_regval_tbl[reg >> 9];
}

void wm8988_set_out_valume(struct rt_i2c_bus_device *dev, rt_uint8_t val)
{
    wm8988_reg_wrtie(dev, LOUT1_VOLUME | (1<<8) | (1<<7) | ((128*val)/100));
    wm8988_reg_wrtie(dev, ROUT1_VOLUME | (1<<8) | (1<<7) | ((128*val)/100));
}
void wm8988_init(struct rt_i2c_bus_device *dev)
{
    /* reset */
    wm8988_reg_wrtie(dev, RESET | 0x0);
    rt_thread_mdelay(100);

    /* power up */

    /* Enable Vmid and VREF. */
    wm8988_reg_wrtie(dev, PWR_MGMT_1 | 0xFC);
    /* Enable DACs as required */
    /* Enable line and / or headphone output buffers as required. */
    wm8988_reg_wrtie(dev, PWR_MGMT_2 | 0x1E0);

    rt_thread_mdelay(100);

    /* Set DACMU = 0 to soft-un-mute the audio DACs */
    wm8988_reg_wrtie(dev, ADC_DAC_CONTROL | 0x4);
    /* Thermal shutdown enabled */
    wm8988_reg_wrtie(dev, ADDITIONAL_CONTROL_1 | 0x1C0);
    wm8988_reg_wrtie(dev, ADDITIONAL_CONTROL_2 | 0x80);
    /* Disable master mode 16-bit */
    wm8988_reg_wrtie(dev, AUDIO_INTERFACE | 0x2);

    wm8988_reg_wrtie(dev, LEFT_OUT_MIX_1 | 0x150);

    wm8988_reg_wrtie(dev, LEFT_OUT_MIX_2 | 0x150);

    wm8988_reg_wrtie(dev, RIGHT_OUT_MIX_1 | 0x150);

    wm8988_reg_wrtie(dev, RIGHT_OUT_MIX_2 | 0x150);

//    wm8988_reg_wrtie(dev, LEFT_DAC_VOLUME | 0x01);

//    wm8988_reg_wrtie(dev, RIGHT_DAC_VOLUME | 0x01);

    wm8988_reg_wrtie(dev, BASS_CONTROL | 0x07);

    wm8988_reg_wrtie(dev, TREBLE_CONTROL | 0x07);

    wm8988_set_out_valume(dev, 58);

    wm8988_reg_read(dev, 0x00);
}
