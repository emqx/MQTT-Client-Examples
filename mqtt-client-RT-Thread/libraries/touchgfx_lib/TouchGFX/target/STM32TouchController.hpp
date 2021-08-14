/**
  ******************************************************************************
  * File Name          : STM32TouchController.hpp
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

/* USER CODE BEGIN STM32TouchController */

#ifndef STM32TOUCHCONTROLLER_HPP
#define STM32TOUCHCONTROLLER_HPP

#include <platform/driver/touch/TouchController.hpp>

/**
 * @class STM32TouchController
 *
 * @brief This class specializes TouchController Interface.
 *
 * @sa touchgfx::TouchController
 */

class STM32TouchController : public touchgfx::TouchController
{
public:

    STM32TouchController() {}

    /**
      * @fn virtual void STM32TouchController::init() = 0;
      *
      * @brief Initializes touch controller.
      *
      *        Initializes touch controller.
      */
    virtual void init();

    /**
    * @fn virtual bool STM32TouchController::sampleTouch(int32_t& x, int32_t& y) = 0;
    *
    * @brief Checks whether the touch screen is being touched, and if so, what coordinates.
    *
    *        Checks whether the touch screen is being touched, and if so, what coordinates.
    *
    * @param [out] x The x position of the touch
    * @param [out] y The y position of the touch
    *
    * @return True if a touch has been detected, otherwise false.
    */
    virtual bool sampleTouch(int32_t& x, int32_t& y);
};

#endif // STM32TOUCHCONTROLLER_HPP

/* USER CODE END STM32TouchController */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
