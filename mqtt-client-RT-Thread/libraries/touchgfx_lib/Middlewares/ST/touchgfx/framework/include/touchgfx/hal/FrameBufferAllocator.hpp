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
 * @file touchgfx/hal/FrameBufferAllocator.hpp
 *
 * Declares the touchgfx::FrameBufferAllocator, touchgfx::SingleBlockAllocator
 * and touchgfx::ManyBlockAllocator classes.
 */
#ifndef FRAMEBUFFERALLOCATOR_HPP
#define FRAMEBUFFERALLOCATOR_HPP
#include <touchgfx/Utils.hpp>

namespace touchgfx
{
/**
 * Called by FrameBufferAllocator to wait for a LCD Transfer, when the allocator has no free
 * blocks. The LCD driver can use this function to synchronize the UI thread with the
 * transfer logic.
 */
void FrameBufferAllocatorWaitOnTransfer();

/**
 * Called by FrameBufferAllocator when a block is drawn and therefore ready for transfer. The
 * LCD driver should use this method to start a transfer.
 */
void FrameBufferAllocatorSignalBlockDrawn();

/**
 * This class is an abstract interface for a class allocating partial framebuffer blocks. The
 * interface must be implemented by a subclass.
 *
 * @see ManyBlockAllocator
 */
class FrameBufferAllocator
{
public:
    /**
     * Allocates a framebuffer block. The block will have at least the width requested. The
     * height of the allocated block can be lower than requested if not enough memory is
     * available.
     *
     * @param          x      The absolute x coordinate of the block on the screen.
     * @param          y      The absolute y coordinate of the block on the screen.
     * @param          width  The width of the block.
     * @param          height The height of the block.
     * @param [in,out] block  Pointer to pointer to return the block address in.
     *
     * @return The height of the allocated block.
     */
    virtual uint16_t allocateBlock(const uint16_t x, const uint16_t y, const uint16_t width, const uint16_t height, uint8_t** block) = 0;

    /**
     * Marks a previously allocated block as ready to be transferred to the LCD.
     *
     */
    virtual void markBlockReadyForTransfer() = 0;

    /**
     * Check if a block is ready for transfer to the LCD.
     *
     * @return True if a block is ready for transfer.
     */
    virtual bool hasBlockReadyForTransfer() = 0;

    /**
     * Get the block ready for transfer.
     *
     * @param [out] rect Reference to rect to write block x, y, width, and height.
     *
     * @return Returns the address of the block ready for transfer.
     */
    virtual const uint8_t* getBlockForTransfer(Rect& rect) = 0;

    /**
     * Get the Rect of the next block to transfer.
     *
     * @return Rect ready for transfer.
     *
     * @see hasBlockReadyForTransfer
     *
     * @note This function should only be called when the allocator has a block ready for transfer.
     */
    virtual const Rect& peekBlockForTransfer() = 0;

    /**
     * Check if a block is ready for drawing (the block is empty).
     *
     * @return True if a block is empty.
     */
    virtual bool hasEmptyBlock() = 0;

    /**
     * Free a block after transfer to the LCD. Marks a previously allocated block as
     * transferred and ready to reuse.
     */
    virtual void freeBlockAfterTransfer() = 0;

    /** Finalizes an instance of the FrameBufferAllocator class. */
    virtual ~FrameBufferAllocator()
    {
    }

protected:
    /** BlockState is used for internal state of each block. */
    enum BlockState
    {
        EMPTY,     ///< Block is empty, can be allocated
        ALLOCATED, ///< Block is allocated for drawing
        DRAWN,     ///< Block has been drawn to, can be send
        SENDING    ///< Block is being transmitted to the display
    };
};

/**
 * This class is partial framebuffer allocator using multiple blocks. New buffers can be
 * allocated until no free blocks are available. After transfer to LCD, a block is
 * queued for allocation again.
 *
 * @see FrameBufferAllocator
 */
template <uint32_t block_size, uint32_t blocks, uint32_t bytes_pr_pixel>
class ManyBlockAllocator : public FrameBufferAllocator
{
public:
    ManyBlockAllocator()
    {
        sendingBlock = -1;
        drawingBlock = -1;
        for (uint32_t i = 0; i < blocks; i++)
        {
            state[i] = EMPTY;
        }
    }

    /**
     * Allocates a framebuffer block. The block will have at least the width requested. The
     * height of the allocated block can be lower than requested if not enough memory is
     * available.
     *
     * @param          x      The absolute x coordinate of the block on the screen.
     * @param          y      The absolute y coordinate of the block on the screen.
     * @param          width  The width of the block.
     * @param          height The height of the block.
     * @param [in,out] block  Pointer to pointer to return the block address in.
     *
     * @return The height of the allocated block.
     */
    virtual uint16_t allocateBlock(const uint16_t x, const uint16_t y, const uint16_t width, const uint16_t height, uint8_t** block)
    {
        drawingBlock++;
        if (drawingBlock == blocks)
        {
            drawingBlock = 0;
        }
        while (state[drawingBlock] != EMPTY)
        {
            FrameBufferAllocatorWaitOnTransfer();
        }
        assert(state[drawingBlock] == EMPTY);
        state[drawingBlock] = ALLOCATED;
        const int32_t stride = width * bytes_pr_pixel;
        const int32_t lines = block_size / stride;
        *block = (uint8_t*)&memory[drawingBlock][0];
        blockRect[drawingBlock].x = x;
        blockRect[drawingBlock].y = y;
        blockRect[drawingBlock].width = width;
        blockRect[drawingBlock].height = MIN(height, lines);
        return blockRect[drawingBlock].height;
    }

    /** Marks a previously allocated block as ready to be transferred to the LCD. */
    virtual void markBlockReadyForTransfer()
    {
        assert(state[drawingBlock] == ALLOCATED);
        state[drawingBlock] = DRAWN;
        FrameBufferAllocatorSignalBlockDrawn();
    }

    /**
     * Check if a block is ready for transfer to the LCD.
     *
     * @return True if a block is ready for transfer.
     */
    virtual bool hasBlockReadyForTransfer()
    {
        for (uint32_t i = 0; i < blocks; i++)
        {
            if (state[i] == DRAWN)
            {
                return true;
            }
        }
        return false;
    }

    /**
     * Get the block ready for transfer.
     *
     * @param [in,out] rect Reference to rect to write block x, y, width, and height.
     *
     * @return Returns the address of the block ready for transfer.
     */
    virtual const uint8_t* getBlockForTransfer(Rect& rect)
    {
        sendingBlock++;
        if (sendingBlock == blocks)
        {
            sendingBlock = 0;
        }
        assert(state[sendingBlock] == DRAWN);
        rect = blockRect[sendingBlock];
        state[sendingBlock] = SENDING;
        return (const uint8_t*)&memory[sendingBlock][0];
    }

    virtual const Rect& peekBlockForTransfer()
    {
        int nextSendingBlock = sendingBlock + 1;
        if (nextSendingBlock == blocks)
        {
            nextSendingBlock = 0;
        }
        assert(state[nextSendingBlock] == DRAWN);
        return blockRect[nextSendingBlock];
    }

    virtual bool hasEmptyBlock()
    {
        int nextDrawingBlock = drawingBlock + 1;
        if (nextDrawingBlock == blocks)
        {
            nextDrawingBlock = 0;
        }
        return (state[nextDrawingBlock] == EMPTY);
    }

    /**
     * Free a block after transfer to the LCD.
     *
     * Marks a previously allocated block as transferred and ready to reuse.
     */
    virtual void freeBlockAfterTransfer()
    {
        assert(state[sendingBlock] == SENDING);
        state[sendingBlock] = EMPTY;
    }

private:
    volatile BlockState state[blocks];
    uint32_t memory[blocks][block_size / 4];
    Rect blockRect[blocks];
    int sendingBlock;
    int drawingBlock;
};

} // namespace touchgfx

#endif // FRAMEBUFFERALLOCATOR_HPP
