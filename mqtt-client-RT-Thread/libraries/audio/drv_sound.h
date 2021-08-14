/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author         Notes
 * 2020-05-15     liuduanfei     first version
 */

#ifndef __DRV_SOUND_H_ 
#define __DRV_SOUND_H_ 

#include <rtthread.h> 
#include <rtdevice.h>
#include <board.h> 

#define AUDIO_FREQUENCY_008K ((rt_uint32_t)   8000)
#define AUDIO_FREQUENCY_011K ((rt_uint32_t)  11025)
#define AUDIO_FREQUENCY_016K ((rt_uint32_t)  16000)
#define AUDIO_FREQUENCY_022K ((rt_uint32_t)  22050)
#define AUDIO_FREQUENCY_032K ((rt_uint32_t)  32000)
#define AUDIO_FREQUENCY_044K ((rt_uint32_t)  44100)
#define AUDIO_FREQUENCY_048K ((rt_uint32_t)  48000)
#define AUDIO_FREQUENCY_088K ((rt_uint32_t)  88200)
#define AUDIO_FREQUENCY_096K ((rt_uint32_t)  96000)
#define AUDIO_FREQUENCY_174K ((rt_uint32_t) 174000)
#define AUDIO_FREQUENCY_192K ((rt_uint32_t) 192000)

struct sai_reg
{
    volatile rt_uint32_t gcr;            /* 0x00 */
    volatile rt_uint32_t acr1;           /* 0x04 */
    volatile rt_uint32_t acr2;           /* 0x08 */
    volatile rt_uint32_t afrcr;          /* 0x0c */
    volatile rt_uint32_t aslotr;         /* 0x10 */
    volatile rt_uint32_t aim;            /* 0x14 */
    volatile rt_uint32_t asr;            /* 0x18 */
    volatile rt_uint32_t aclrfr;         /* 0x1c */
    volatile rt_uint32_t adr;            /* 0x20 */
    volatile rt_uint32_t bcr1;           /* 0x24 */
    volatile rt_uint32_t bcr2;           /* 0x28 */
    volatile rt_uint32_t bfrcr;          /* 0x2c */
    volatile rt_uint32_t bslotr;         /* 0x30 */
    volatile rt_uint32_t bim;            /* 0x34 */
    volatile rt_uint32_t bsr;            /* 0x38 */
    volatile rt_uint32_t bclrfr;         /* 0x3c */
    volatile rt_uint32_t bdr;            /* 0x40 */
    volatile rt_uint32_t pdmcr;          /* 0x44 */
    volatile rt_uint32_t pdmdly;         /* 0x48 */
};

#define SAI1BASE (0x40015800)
#define SAI2BASE (0x40015C00)

void SAIA_samplerate_set(rt_uint32_t samplerate);
void SAIA_channels_set(rt_uint16_t channels);
void SAIA_samplebits_set(rt_uint16_t samplebits);
void SAIA_config_set(struct rt_audio_configure config);


#endif
