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
 * @file touchgfx/hal/DMA.hpp
 *
 * Declares the touchgfx::DMA_Queue (abstract), touchgfx::LockFreeDMA_Queue and
 * touchgfx::DMA_Interface (abstract) classes.
 */
#ifndef DMA_HPP
#define DMA_HPP

#include <touchgfx/hal/Atomic.hpp>
#include <touchgfx/hal/BlitOp.hpp>

namespace touchgfx
{
class DMA_Interface;

/**
 * This class provides an interface for a FIFO (circular) list used by DMA_Interface and
 * descendants for storing BlitOp's.
 */
class DMA_Queue
{
    friend class DMA_Interface;

public:
    /**
     * Query if this object is empty.
     *
     * @return true if the queue is empty.
     */
    virtual bool isEmpty() = 0;

    /**
     * Query if this object is full.
     *
     * @return true if the queue is full.
     */
    virtual bool isFull() = 0;

    /**
     * Adds the specified blitop to the queue.
     *
     * @param  op The blitop to add.
     */
    virtual void pushCopyOf(const BlitOp& op) = 0;

    /** Finalizes an instance of the DMA_Queue class. */
    virtual ~DMA_Queue()
    {
    }

protected:
    /** Initializes a new instance of the DMA_Queue class. */
    DMA_Queue()
    {
    }

    /** Pops an element from the queue. */
    virtual void pop() = 0;

    /**
     * Gets the first element in the queue.
     *
     * @return The first element in the queue.
     */
    virtual const BlitOp* first() = 0;
};

/**
 * This implements a simple lock-free FIFO queue (single producer, single consumer)
 *
 * @see DMA_Queue
 */
class LockFreeDMA_Queue : public DMA_Queue
{
public:
    /**
     * Constructs a lock-free queue.
     *
     * @param [out] mem Pointer to the memory used by the queue to store elements.
     * @param       n   Number of elements the memory provided can contain.
     */
    LockFreeDMA_Queue(BlitOp* mem, atomic_t n);

    virtual bool isEmpty();

    virtual bool isFull();

    virtual void pushCopyOf(const BlitOp& op);

protected:
    virtual void pop();

    virtual const BlitOp* first();

    BlitOp* q;         ///< Pointer to the queue memory.
    atomic_t capacity; ///< The number of elements the queue can contain.
    atomic_t head;     ///< Index to the head element.
    atomic_t tail;     ///< Index to the tail element.
};

/**
 * DMA_Interface provides basic functionality and structure for processing "blit" operations
 * using DMA.
 */
class DMA_Interface
{
public:
    /**
     * Gets the blit capabilities of this DMA.
     *
     * @return The blit operations supported by this DMA implementation.
     */
    virtual BlitOperations getBlitCaps() = 0;

    /**
     * Inserts a BlitOp for processing. This also potentially starts the DMA controller, if
     * not already running.
     *
     * @param  op The operation to add.
     */
    virtual void addToQueue(const BlitOp& op);

    /** This function blocks until all DMA transfers in the queue have been completed. */
    virtual void flush()
    {
        waitForFrameBufferSemaphore();
    }

    /** Perform initialization. Does nothing in this base class. */
    virtual void initialize()
    {
    }

    /**
     * Query if the DMA is running.
     *
     * @return true if a DMA operation is currently in progress.
     */
    bool isDMARunning()
    {
        return isRunning;
    }

    /**
     * Sets whether or not a DMA operation is allowed to begin. Used in single-buffering to
     * avoid changing the framebuffer while display is being updated.
     *
     * @param  allowed true if DMA transfers are allowed.
     */
    void setAllowed(bool allowed)
    {
        isAllowed = allowed;
    }

    /**
     * Gets whether a DMA operation is allowed to begin. Used in single-buffering to avoid
     * changing the framebuffer while display is being updated.
     *
     * @return true if DMA is allowed to start, false if not.
     */
    bool getAllowed() const
    {
        return isAllowed;
    }

    /** Signals that DMA transfers can start. If any elements are in the queue, start it. */
    virtual void start();

    /**
     * This function is called automatically by the framework when a DMA interrupt has been
     * received.
     *
     * This function is called automatically by the framework when a DMA interrupt has been
     * received.
     */
    virtual void signalDMAInterrupt() = 0;

    /**
     * Query if the DMA queue is empty.
     *
     * @return 1 if DMA queue is empty, else 0.
     */
    uint8_t isDmaQueueEmpty();

    /**
     * Query if the DMA queue is full.
     *
     * @return 1 if DMA queue is full, else 0.
     */
    uint8_t isDmaQueueFull();

    /**
     * Function for obtaining the DMA type of the concrete DMA_Interface implementation. As
     * default, will return DMA_TYPE_GENERIC type value.
     *
     * @return a DMAType value of the concrete DMA_Interface implementation.
     */
    virtual DMAType getDMAType(void)
    {
        return DMA_TYPE_GENERIC;
    }

    /** Finalizes an instance of the DMA_Interface class. */
    virtual ~DMA_Interface()
    {
    }

protected:
    /**
     * Constructs a DMA Interface object.
     *
     * @param [in] dmaQueue Reference to the queue of DMA operations.
     */
    DMA_Interface(DMA_Queue& dmaQueue)
        : queue(dmaQueue), isRunning(false), isAllowed(false)
    {
    }

    /** Performs a queued blit-op. */
    virtual void execute();

    /** To be called when blit-op has been performed. */
    virtual void executeCompleted();

    /**
     * Called when elements are added to the DMA-queue.
     *
     * @note The framebuffer must be locked before this method returns if the DMA-queue is non-
     *       empty.
     */
    virtual void seedExecution();

    /**
     * Configures blit-op hardware for a 2D copy as specified by blitOp.
     *
     * @param  blitOp The operation to execute.
     */
    virtual void setupDataCopy(const BlitOp& blitOp) = 0;

    /**
     * Configures blit-op hardware for a 2D fill as specified by blitOp.
     *
     * @param  blitOp The operation to execute.
     */
    virtual void setupDataFill(const BlitOp& blitOp) = 0;

    /**
     * Configures blit-op hardware for alpha-blending.
     *
     * @param  alpha The alpha-blending value to apply.
     */
    virtual void enableAlpha(uint8_t alpha);

    /** Configures blit-op hardware for solid operation (no alpha-blending) */
    virtual void disableAlpha();

    /**
     * Configures blit-op hardware for alpha-blending while simultaneously skipping
     * transparent pixels.
     *
     * @param  alpha The alpha-blending value to apply.
     */
    virtual void enableCopyWithTransparentPixels(uint8_t alpha);

    /**
     * Waits until framebuffer semaphore is available (i.e. neither DMA or application is
     * accessing the framebuffer).
     */
    virtual void waitForFrameBufferSemaphore();

    DMA_Queue& queue;        ///< Reference to the DMA queue
    bool isRunning;          ///< true if a DMA transfer is currently ongoing.
    volatile bool isAllowed; ///< true if DMA transfers are currently allowed.
};

} // namespace touchgfx

#endif // DMA_HPP
