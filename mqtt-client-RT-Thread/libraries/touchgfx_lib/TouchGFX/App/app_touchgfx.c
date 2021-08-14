/**
  ******************************************************************************
  * File Name          : app_touchgfx.c
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include "app_touchgfx.h"

void touchgfx_init(void);
void touchgfx_taskEntry(void);

/**
 * Initialize TouchGFX application
 */
void MX_TouchGFX_Init(void)
{
  // Calling farward to touchgfx_init in C++ domain
  touchgfx_init();
}

/**
 * TouchGFX application entry function
 */
void MX_TouchGFX_Process(void)
{
  // Calling farward to touchgfx_init in C++ domain
  touchgfx_taskEntry();
}

/**
 * TouchGFX application thread
 */
void TouchGFX_Task(void *argument)
{
  // Calling farward to touchgfx_init in C++ domain
  touchgfx_taskEntry();
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
