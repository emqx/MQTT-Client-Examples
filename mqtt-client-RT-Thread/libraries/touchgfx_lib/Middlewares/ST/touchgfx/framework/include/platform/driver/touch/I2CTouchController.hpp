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
 * @file platform/driver/touch/I2CTouchController.hpp
 *
 * Declares the touchgfx::I2CTouchController interface class.
 */
#ifndef I2CTOUCHCONTROLLER_HPP
#define I2CTOUCHCONTROLLER_HPP

#include <platform/driver/i2c/I2C.hpp>
#include <platform/driver/touch/TouchController.hpp>
#include <touchgfx/hal/Types.hpp>

namespace touchgfx
{
/**
 * Specific I2C-enabled type of Touch Controller.
 *
 * @see TouchController
 */
class I2CTouchController : public TouchController
{
public:
    /**
     * Constructor. Initializes I2C driver.
     *
     * @param [in] i2c I2C driver.
     */
    I2CTouchController(I2C& i2c)
        : i2c(i2c)
    {
        i2c.init();
    }

    virtual ~I2CTouchController()
    {
    }

    virtual void init() = 0;

    virtual bool sampleTouch(int32_t& x, int32_t& y) = 0;

protected:
    I2C& i2c; ///< I2C driver
};

} // namespace touchgfx

#endif // I2CTOUCHCONTROLLER_HPP
