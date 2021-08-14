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
 * @file touchgfx/canvas_widget_renderer/CanvasWidgetRenderer.hpp
 *
 * Declares the touchgfx::CanvasWidgetRenderer class.
 */
#ifndef CANVASWIDGETRENDERER_HPP
#define CANVASWIDGETRENDERER_HPP

#include <touchgfx/canvas_widget_renderer/Cell.hpp>
#include <touchgfx/widgets/Widget.hpp>
#ifdef SIMULATOR
#include <stdio.h>
#endif

namespace touchgfx
{
/**
 * Class for supporting drawing of figures. This class holds the memory which is used by the
 * underlying algorithms. CanvasWidget will not allocate memory dynamically, but will
 * use memory from the buffer passed to CanvasWidgetRenderer. When using the TouchGFX
 * simulator, it is also possible to get a report on the actual amount of memory used
 * for drawing with CanvasWidgetRenderer to help adjusting the buffer size.
 *
 * @see Widget, setWriteMemoryUsageReport, getWriteMemoryUsageReport
 */
class CanvasWidgetRenderer : public Widget
{
public:
    /**
     * Setup the buffers used by CanvasWidget.
     *
     * @param [in] buffer  Buffer reserved for CanvasWidget.
     * @param      bufsize The size of the buffer.
     */
    static void setupBuffer(uint8_t* buffer, unsigned bufsize);

    /// @cond
    /**
     * Sets scanline width. Setting the scanline width will initialize the buffers for
     * scanline and outline. If the width set is too large to hold the scanline buffers in
     * the allocated memory buffer, false will be returned and all buffer pointers will be
     * cleared.
     *
     * @param  width The width of the scanline on screen.
     *
     * @return true if it succeeds, false if it fails.
     */
    static bool setScanlineWidth(unsigned width);

    /**
     * Query if CanvasWidgetRenderer has been initialized with a buffer.
     *
     * @return True if a buffer has been setup.
     */
    static bool hasBuffer();

    /**
     * The width of a scanline. This is the same as the width of the invalidated area. Used
     * to optimize the memory layout of the buffer.
     *
     * @return Scanline width (HAL::FRAME_BUFFER_WIDTH).
     */
    static unsigned getScanlineWidth();

    /**
     * Gets pointer to memory used for covers in Scanline.
     *
     * @return Pointer to memory used internally by Scanline.
     */
    static void* getScanlineCovers();

    /**
     * Gets pointer to memory used for indices in Scanline.
     *
     * @return Pointer to memory used internally by Scanline.
     */
    static void* getScanlineStartIndices();

    /**
     * Gets pointer to memory used for counts in Scanline.
     *
     * @return Pointer to memory used internally by Scanline.
     */
    static void* getScanlineCounts();

    /**
     * Gets pointer to memory used for Cell objects in Outline.
     *
     * @return Pointer to memory used internally by Outline.
     */
    static Cell* getOutlineBuffer();

    /**
     * Gets size of memory area used for Cell objects in Outline.
     *
     * @return Size of memory area used internally by Outline.
     */
    static unsigned int getOutlineBufferSize();
    /// @endcond

#ifdef SIMULATOR

    /**
     * Memory reporting.
     *
     * Memory reporting can be turned on (and off) using this method. CWR will try to work
     * with the given amount of memory passed when calling setupBuffer(). If the outline of
     * the figure is too complex, this will be reported.
     *
     * "CWR requires X bytes" means that X bytes is the highest number of bytes required by
     * CWR so far, but since the size of the invalidated area and the shape of things draw
     * can influence this, this may be reported several times with a higher and higher
     * number. Leave your app running for a long time to find out what the memory
     * requirements are.
     *
     * "CWR requires X bytes (Y bytes missing)" means the same as the report above, but
     * there as was not enough memory to render the entire shape. To get around this, CWR
     * will split the shape into two separate drawings of half size. This means that less
     * memory is required, but drawing will be (somewhat)
     * slower. After you see this message all future draws will be split into smaller chunks,
     * so memory requirements might not get as high. This is followed by:
     *
     * "CWR will split draw into multiple draws due to limited memory." actually just means
     * that CWR will try to work with a smaller amount of memory.
     *
     * In general, if there is enough memory available to run the simulation and never see
     * the message "CWR will split draw ...", this is preferred. The size of the buffer
     * required will be the highest number X reported as "CWR requires X bytes". Good
     * numbers can also be around half of X.
     *
     * @param  writeUsageReport true to write report.
     *
     * @see setupBuffer
     */
    static void setWriteMemoryUsageReport(bool writeUsageReport);

    /**
     * Gets write memory usage report flag.
     *
     * @return true if it CWR writes memory reports, false if not.
     */
    static bool getWriteMemoryUsageReport();

    /// @cond
    /**
     * Called after a shape (Circle, Line, etc.) has been drawn to keep track of the memory
     * requirements of CanvasWidgets.
     *
     * @param  used Number of Cell objects used from the dedicated buffer.
     */
    static void numCellsUsed(unsigned used);

    /**
     * Called after a shape (Circle, Line, etc.) has been drawn to keep track of the memory
     * requirements of CanvasWidgets.
     *
     * @param  missing Number of Cell objects required, but not available.
     */
    static void numCellsMissing(unsigned missing);
    /// @endcond

    /**
     * Calculate how much memory has been required by CanvasWidgets. This can be used to
     * fine tune the size of the buffer passed to CanvasWidgetRenderer upon initialization.
     *
     * @return The number of bytes required.
     */
    static unsigned getUsedBufferSize();

    /**
     * Calculate how much memory was required by CanvasWidgets, but was unavailable. If the
     * value returned is greater than 0 it means the This can be used to fine tune the size
     * of the buffer passed to CanvasWidgetRenderer upon initialization.
     *
     * @return The number of bytes required.
     */
    static unsigned getMissingBufferSize();

#endif

private:
    static uint8_t* memoryBuffer;
    static unsigned int memoryBufferSize;
    static unsigned int scanlineWidth;
    static void* scanlineCovers;
    static void* scanlineStartIndices;
    static void* scanlineCounts;
    static Cell* outlineBuffer;
    static unsigned int outlineBufferSize;
#ifdef SIMULATOR
    static unsigned int scanlineSize;
    static unsigned int maxCellsUsed;
    static unsigned int maxCellsMissing;
    static bool writeReport;
#endif
};

} // namespace touchgfx

#endif // CANVASWIDGETRENDERER_HPP
