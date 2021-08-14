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

#include <platform/driver/touch/ST1232TouchController.hpp>

#ifndef NO_USING_NAMESPACE_TOUCHGFX
using namespace touchgfx;
#endif

void touchgfx::ST1232TouchController::init()
{
}

/*
//This method does not allow us to return more than a single coordinate
//Should be enhanced to track multiple fingers. => HAL TYPES.
// Depending on TouchControl setup - allow tracking of

1) Coordinates for Fingers <= MAX NR OF FINGERS
2) Gestures for Fingers <= MAX NR OF FINGERS

*/

static Finger finger;
bool touchgfx::ST1232TouchController::sampleTouch(int32_t& x, int32_t& y)
{
    //For each configured and valid finger, collect 1) Coordinates  2) Gestures (if configured)
    uint8_t fingersOnDisplay = touchDetected();
    if (fingersOnDisplay)
    {
        //Get sample for finger 1 and extract data
        if (getSample(&finger))
        {
            x = finger.x;
            y = finger.y;

            //Apply transformations
            transformTouchData(x, y);

            return true;
        }
    }
    return false;
}

void touchgfx::ST1232TouchController::transformTouchData(int32_t& x, int32_t& y)
{

    /* Example of linear transformations for various touch controller configurations
    Screen width in example is 480 and height is 272.                             */

    /***************************************
     X-axis is flipped
    ****************************************
    * (0,272)                      (480,272)
    *
    *
    *
    * (0,0)                          (480,0)
    ***************************************/
    // y = 272 - y;

    /***************************************
      Y-axis flipped
    ****************************************
    * (480,0)                          (0,0)
    *
    *
    *
    * (480,272)                      (0,272)
    ***************************************/
    //x = 480 - x

    //If transformation is configured, apply it to sampled coordinates
    switch (linearTransform)
    {
    case FLIP_X_AXIS:
        y = displayHeight - y;
        break;
    case FLIP_Y_AXIS:
        x = displayWidth - x;
        break;
    case NO_TRANSFORM:
        //do nothing
        break;
    }

    //If user defined a custom method for transformation elsewhere
    if (transformFunc != 0)
    {
        transformFunc(x, y, finger);
    }
}


uint8_t touchgfx::ST1232TouchController::touchDetected(void)
{
    uint8_t touched = 0;

    //Read 1 byte of REG_FINGERS_GESTURE register into touched
    i2c.readRegister(I2C_ADDR, REG_FINGERS_GESTURE, (uint8_t*)&touched, 1);

    //First 3 bytes define the number of fingers on display.
    return touched & MASK_NR_OF_FINGERS;
}

bool touchgfx::ST1232TouchController::getSample(Finger* touchData)
{
    uint8_t xy_low_byte, xy_high_byte;

    //Read XY High byte and verify validty of data
    i2c.readRegister(I2C_ADDR, REG_XY0_COORD_HIGH_BYTE, &xy_high_byte, 1);

    //While one or more fingers return valid touchdata
    if (xy_high_byte & MASK_MSB)
    {
        //Read the X low byte
        i2c.readRegister(I2C_ADDR, REG_X0_COORD_LOW_BYTE, &xy_low_byte, 1);

        //Add X high and low bytes. Located at the top 4 bits of register.
        touchData->x = ((xy_high_byte & MASK_X0_COORD_LOW_BYTE) << (SIZE_BYTES / 2)) | xy_low_byte;

        //Read the Y low byte
        i2c.readRegister(I2C_ADDR, REG_Y0_COORD_LOW_BYTE, &xy_low_byte, 1);

        //Add Y high and low bytes Located at the bottom 4 bits of register.
        touchData->y = ((xy_high_byte & MASK_Y0_COORD_LOW_BYTE) << SIZE_BYTES) | xy_low_byte;

        //Touchdata was valid
        return true;
    }
    //Touchdata was valid
    return false;
}
