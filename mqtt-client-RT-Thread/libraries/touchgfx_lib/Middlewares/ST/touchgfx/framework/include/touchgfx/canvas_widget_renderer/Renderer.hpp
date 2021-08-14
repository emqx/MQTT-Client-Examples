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
 * @file touchgfx/canvas_widget_renderer/Renderer.hpp
 *
 * Declares the touchgfx::Renderer class. Used internally by CanvasWidgetRenderer.
 */
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <touchgfx/canvas_widget_renderer/RenderingBuffer.hpp>
#include <touchgfx/canvas_widget_renderer/Scanline.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/widgets/canvas/AbstractPainter.hpp>

/// @cond
namespace touchgfx
{
/**
 * This class template is used basically for rendering scanlines. The 'Span' argument is one of
 * the span renderers, such as SpanRGB565 and others.
 */
class Renderer
{
public:
    /**
     * Initializes a new instance of the Renderer class.
     *
     * @param [in] renderingBuffer The screen buffer to render the polygon in.
     * @param [in] painter         The painter to use for drawing individual pixels in a
     *                             scanline.
     */
    Renderer(RenderingBuffer& renderingBuffer, AbstractPainter& painter)
        : renderingBuffer(&renderingBuffer), painter(&painter)
    {
    }

    /**
     * Sets rendering buffer.
     *
     * @param [in] renderingBuffer The screen buffer to render the polygon in.
     */
    void setRenderingBuffer(RenderingBuffer& renderingBuffer)
    {
        this->renderingBuffer = &renderingBuffer;
    }

    /**
     * Render the given Scanline in the given color.
     *
     * @param  scanline The Scanline.
     */
    void render(const Scanline& scanline)
    {
        if (scanline.getY() < 0 || scanline.getY() >= int(renderingBuffer->getHeight()))
        {
            return;
        }

        unsigned numSpans = scanline.getNumSpans();
        int baseX = 0;
        int y = scanline.getY();
        unsigned char* row = renderingBuffer->row(y);
        Scanline::iterator spanIterator(scanline);

        uint8_t xAdjust = renderingBuffer->getXAdjust();
        do
        {
            int x = spanIterator.next() + baseX;
            const uint8_t* covers = spanIterator.getCovers();
            int numPix = spanIterator.getNumPix();
            if (x < 0)
            {
                numPix += x;
                if (numPix <= 0)
                {
                    continue;
                }
                covers -= x;
                x = 0;
            }
            if (x + numPix >= int(renderingBuffer->getWidth()))
            {
                numPix = renderingBuffer->getWidth() - x;
                if (numPix <= 0)
                {
                    continue;
                }
            }
            painter->render(row, x, xAdjust, y, numPix, covers);
        } while (--numSpans);
    }

    /**
     * Gets the getRenderingBuffer.
     *
     * @return A RenderingBuffer&amp;
     */
    RenderingBuffer& getRenderingBuffer()
    {
        return *renderingBuffer;
    }

private:
    RenderingBuffer* renderingBuffer; ///< Buffer for rendering data
    AbstractPainter* painter;         ///< The painter
};

} // namespace touchgfx
/// @endcond

#endif // RENDERER_HPP
