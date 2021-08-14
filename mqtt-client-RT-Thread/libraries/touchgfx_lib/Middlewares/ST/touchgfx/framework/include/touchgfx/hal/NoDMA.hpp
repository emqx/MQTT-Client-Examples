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
 * @file touchgfx/hal/NoDMA.hpp
 *
 * Declares the touchgfx::NoDMA class.
 */
#ifndef NODMA_HPP
#define NODMA_HPP

#include <assert.h>
#include <touchgfx/hal/DMA.hpp>

namespace touchgfx
{
/**
 * This is an "empty" DMA subclass that does nothing except assert if accidentally used. An
 * instance of this object can be used if DMA support is not desired.
 *
 * @see DMA_Interface
 */
class NoDMA : public DMA_Interface
{
public:
    NoDMA()
        : DMA_Interface(q), q(&b, 1)
    {
    }

    /**
     * No blit operations supported by this DMA implementation.
     *
     * @return Zero (no blit ops supported).
     */
    virtual BlitOperations getBlitCaps()
    {
        return static_cast<BlitOperations>(0);
    }

    /**
     * Asserts if used.
     *
     * @param  blitOp The blit operation to be performed by this DMA instance.
     */
    virtual void setupDataCopy(const BlitOp& blitOp)
    {
        assert(0 && "DMA operation not supported");
    }

    /**
     * Asserts if used.
     *
     * @param  blitOp The blit operation to be performed by this DMA instance.
     */
    virtual void setupDataFill(const BlitOp& blitOp)
    {
        assert(0 && "DMA operation not supported");
    }

    /** Does nothing. */
    virtual void signalDMAInterrupt()
    {
    }

    /**
     * Block until all DMA transfers are complete. Since this particular DMA does not do
     * anything, return immediately.
     */
    virtual void flush()
    {
    }

private:
    LockFreeDMA_Queue q;
    BlitOp b;
};

} // namespace touchgfx

#endif // NODMA_HPP
