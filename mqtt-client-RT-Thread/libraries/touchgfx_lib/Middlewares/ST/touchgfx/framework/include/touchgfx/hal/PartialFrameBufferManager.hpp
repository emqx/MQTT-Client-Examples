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
 * @file touchgfx/hal/PartialFrameBufferManager.hpp
 *
 * Declares the touchgfx::PartialFrameBufferManager class.
 */
#ifndef PARTIALFRAMEBUFFERMANAGER_HPP
#define PARTIALFRAMEBUFFERMANAGER_HPP

#include <touchgfx/Utils.hpp>

namespace touchgfx
{
/**
 * Check if a Frame Buffer Block is beeing transmitted.
 *
 * @return Non zero if possible.
 */
int transmitActive();

/**
 * Check if a Frame Buffer Block ending at bottom may be sent.
 *
 * @param bottom The bottom coordinate of the block to transfer.
 *
 * @return Non zero if possible.
 */
int shouldTransferBlock(uint16_t bottom);

/**
 * Transmit a Frame Buffer Block.
 *
 * @param pixels Pointer to the pixel data.
 * @param x      X coordinate of the block.
 * @param y      Y coordinate of the block.
 * @param w      Width of the block.
 * @param h      Height of the block.
 */
void transmitBlock(const uint8_t* pixels, uint16_t x, uint16_t y, uint16_t w, uint16_t h);

/**
 * This class specifies strategies for transmitting block to the display using Partial Frame Buffer.
 */
class PartialFrameBufferManager
{
public:
    /**
     * Transmit all remaining drawn Framebuffer blocks.
     *
     * @note This function does not return before all blocks have been transmitted.
     */
    static void transmitRemainingBlocks();

    /**
     * Tries to transmit a drawn block.
     *
     * @note Will return immediately if already transmitting.
     */
    static void tryTransmitBlock();

    /**
     * Tries to transmit a drawn block in interrupt context.
     *
     *  @note Will transmit next block immediately if drawn.
     */
    static void tryTransmitBlockFromIRQ();
};

}

#endif // PARTIALFRAMEBUFFERMANAGER_HPP
