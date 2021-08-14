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
 * @file platform/driver/touch/ST1232TouchController.hpp
 *
 * Declares the touchgfx::ST1232TouchController class.
 */
#ifndef ST1232TOUCHCONTROLLER_HPP
#define ST1232TOUCHCONTROLLER_HPP

#include <platform/driver/touch/I2CTouchController.hpp>
#include <platform/driver/touch/TouchController.hpp>

/** Represents the coordinates of a finger on the display. */
typedef struct
{
    int16_t x; ///< x coordinate
    int16_t y; ///< y coordinate
    int16_t z; ///< z coordinate
} Finger;

/** Defines the transformation types. */
typedef enum
{
    NO_TRANSFORM = 0, ///< No transformation
    FLIP_X_AXIS,      ///< Swap along the x axis
    FLIP_Y_AXIS       ///< Swap along the y axis
} TransformationType;

//General
#define I2C_ADDR 0x55
#define SIZE_BYTES 8
#define MAX_FINGERS 2
#define MASK_NR_OF_FINGERS 0x07
#define MASK_MSB 0x80
#define MASK_X0_COORD_LOW_BYTE 0x70
#define MASK_Y0_COORD_LOW_BYTE 0x07

// Register addresses
#define REG_FIRMWARE_VERSION 0x00
#define REG_STATUS 0x01
#define REG_DEVICE_CONTROL 0x02
#define REG_TIMEOUT_TO_IDLE 0x03
#define REG_XY_RESOLUTION_HIGH 0x04
#define REG_X_RESOLUTION_L 0x05
#define REG_Y_RESOLUTION_L 0x06
#define REG_KEYS 0x11
#define REG_PAGE 0xFF

//Touch
#define REG_FINGERS_GESTURE 0x10
#define REG_XY0_COORD_HIGH_BYTE 0x12
#define REG_X0_COORD_LOW_BYTE 0x13
#define REG_Y0_COORD_LOW_BYTE 0x14
#define REG_XY1_COORD_HIGH_BYTE 0x15
#define REG_X1_COORD_LOW_BYTE 0x16
#define REG_Y1_COORD_LOW_BYTE 0x17
#define REG_Z0_COORD 0x18
#define REG_Z1_COORD 0x19

//Gestures
/*
#define REG_NO_GESTURE           0x00
#define REG_SINGLE_TOUCH_TAP     0x01
#define SINGLE_TOUCH_DOUBLE_TAP  0x02
#define SINGLE_TOUCH_SLIDE_UP    0x03
#define SINGLE_TOUCH_SLIDE_DOWN  0x04
#define SINGLE_TOUCH_SLIDE_LEFT  0x05
#define SINGLE_TOUCH_SLIDE_RIGHT 0x06
#define TWO_FINGER_SLIDE_UP      0x07
#define TWO_FINGER_SLIDE_DOWN    0x08
#define TWO_FINGER_SLIDE_LEFT    0x09
#define TWO_FINGER_SLIDE_RIGHT   0x0A
#define PINCH_IN                 0x0B
#define PINCH_OUT                0x0C
*/

namespace touchgfx
{
/**
 * ST1232 I2C Touch controller driver.
 *
 * @see I2CTouchController
 */
class ST1232TouchController : public I2CTouchController
{
    /** Function definition for touch correction. */
    typedef void (*TransformFuncPtr)(int32_t&, int32_t&, Finger&);

public:
    /**
     * Initializes a new instance of the ST1232TouchController class.
     *
     * @param [in] i2c           I2C driver handle.
     * @param      displayWidth  Width of the display.
     * @param      displayHeight Height of the display.
     * @param      transType     (Optional) Type of transformation to perform on coordinate
     *                           readout.
     */
    ST1232TouchController(I2C& i2c, int16_t displayWidth, int16_t displayHeight, TransformationType transType = NO_TRANSFORM)
        : I2CTouchController(i2c),
          linearTransform(transType),
          displayWidth(displayWidth),
          displayHeight(displayHeight)
    {
    }

    virtual void init();

    virtual bool sampleTouch(int32_t& x, int32_t& y);

    /**
     * Set pointer to user defined callback method that applies correction to touch
     * coordinates.
     *
     * Set pointer to user defined callback method that applies correction to touch
     * coordinates.
     *
     * @param  ptr Function pointer.
     */
    void setTransformCallback(TransformFuncPtr ptr)
    {
        transformFunc = ptr;
    }

private:
    /**
     * Checks whether 1 or more fingers have been placed on the screen.
     *
     * @return number of fingers.
     */
    uint8_t touchDetected();

    /**
     * Get touchdata for one or more fingers. Currently, this method only tracks 1
     * finger due to HALs notion of sampling touch returns a single coordinate. X and Y
     * Low bytes are full 8 bits.
     *
     * LE bitfield notation for XY_High_Byte for both fingers
     * @code
     *     Y_HIGH_BYTE     : 3
     *     RESERVED        : 1
     *     X_HIGH_BYTE     : 3
     *     VALID           : 1
     *     //XY0 valid.
     * @endcode
     *
     * @param [out] touchData Touchdata.
     *
     * @return true if nothing went wrong, false otherwise.
     */
    bool getSample(Finger* touchData);

    /**
     * Transform touch data based on the linear transformation configuration and user
     * defined transformation, if defined.
     *
     * @param [out] x The x position of the touch.
     * @param [out] y The y position of the touch.
     */
    void transformTouchData(int32_t& x, int32_t& y);

    TransformFuncPtr transformFunc;     ///< Store transform func pointer
    TransformationType linearTransform; ///< Store type of transformation to make on coordinates read from the touch controller.
    int16_t displayWidth;               ///< Store width of display
    int16_t displayHeight;              ///< Store height of display
};

} // namespace touchgfx

#endif // ST1232TOUCHCONTROLLER_HPP
