/**
  ******************************************************************************
  * This file is part of the TouchGFX 4.15.0 distribution.
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

/**
 * @file touchgfx/hal/BoardConfiguration.hpp
 *
 * Declares initialization functions for the hardware as well as for TouchGFX.
 */
#ifndef BOARDCONFIGURATION_HPP
#define BOARDCONFIGURATION_HPP

namespace touchgfx
{
/**
 * Function to perform generic hardware initialization of the board. This function prototype is
 * only provided as a convention.
 */
void hw_init();

/**
 * Function to perform touchgfx initialization. This function prototype is only provided as a
 * convention.
 */
void touchgfx_init();

} // namespace touchgfx

#endif // BOARDCONFIGURATION_HPP
