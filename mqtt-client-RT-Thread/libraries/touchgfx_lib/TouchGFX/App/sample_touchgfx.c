/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-21     KK       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "app_touchgfx.h"
#include <lcd_port.h>


CRC_HandleTypeDef hcrc;

DMA2D_HandleTypeDef hdma2d;

static void MX_CRC_Init(void);
static void MX_DMA2D_Init(void);

#ifdef PKG_USING_GT9147
#include "gt9147.h"
#define GT9147_RST_PIN 51
#define GT9147_IRQ_PIN 108
int rt_hw_gt9147_port(void)
{
    struct rt_touch_config config;
    rt_uint8_t rst;
    rst = GT9147_RST_PIN;
    config.dev_name = "i2c1";
    config.irq_pin.pin  = GT9147_IRQ_PIN;
    config.irq_pin.mode = PIN_MODE_INPUT_PULLDOWN;
    config.user_data = &rst;
    rt_hw_gt9147_init("gt", &config);
    return 0;
}
INIT_ENV_EXPORT(rt_hw_gt9147_port);
#endif


/**
  * @brief This function handles DMA2D global interrupt.
  */
void DMA2D_IRQHandler(void)
{
    rt_enter_critical();

    HAL_DMA2D_IRQHandler(&hdma2d);

    rt_exit_critical();
}
/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{
	__HAL_RCC_CRC_CLK_ENABLE();
	hcrc.Instance = CRC;
	if (HAL_CRC_Init(&hcrc) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
  * @brief DMA2D Initialization Function
  * @param None
  * @retval None
  */
static void MX_DMA2D_Init(void)
{
	__HAL_RCC_DMA2D_CLK_ENABLE();
    hdma2d.Instance = DMA2D;
    hdma2d.Init.Mode = DMA2D_M2M;
    if(LCD_PIXEL_FORMAT == RTGRAPHIC_PIXEL_FORMAT_RGB565)
    {
        hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB565;
        hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_RGB565;
    }
    if(LCD_PIXEL_FORMAT == RTGRAPHIC_PIXEL_FORMAT_RGB888)
    {
        hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB888;
        hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_RGB888;
    }
    hdma2d.Init.OutputOffset = 0;
    hdma2d.LayerCfg[1].InputOffset = 0;
    hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
    hdma2d.LayerCfg[1].InputAlpha = 0;
    hdma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA;
    hdma2d.LayerCfg[1].RedBlueSwap = DMA2D_RB_REGULAR;
    hdma2d.LayerCfg[1].ChromaSubSampling = DMA2D_NO_CSS;
    if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) != HAL_OK)
    {
        Error_Handler();
    }
}
void touchgfx_thread_entry(void *parameter)
{
    MX_CRC_Init();
    MX_DMA2D_Init();
	MX_TouchGFX_Init();
	MX_TouchGFX_Process();
	for(;;)
	{
	    rt_thread_mdelay(1);
	}
}
int TouchGFXTask(void)
{
	rt_thread_t tid = NULL;
	tid = rt_thread_create("TouchGFX",
							touchgfx_thread_entry, RT_NULL,
							8192, 15, 20);
	
   if (tid != RT_NULL)
        rt_thread_startup(tid);
   else
        return -1;

    return RT_EOK;
}
INIT_APP_EXPORT(TouchGFXTask);
