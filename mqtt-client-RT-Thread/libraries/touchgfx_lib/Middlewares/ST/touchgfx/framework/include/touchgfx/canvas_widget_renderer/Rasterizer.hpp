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
 * @file touchgfx/canvas_widget_renderer/Rasterizer.hpp
 *
 * Declares the touchgfx::Rasterizer class. Used internally by CanvasWidgetRenderer.
 */
#ifndef RASTERIZER_HPP
#define RASTERIZER_HPP

#include <touchgfx/canvas_widget_renderer/Outline.hpp>
#include <touchgfx/canvas_widget_renderer/Rasterizer.hpp>
#include <touchgfx/canvas_widget_renderer/Renderer.hpp>
#include <touchgfx/canvas_widget_renderer/Scanline.hpp>

/// @cond
namespace touchgfx
{
/**
 * Polygon Rasterizer that is used to render filled polygons with high-quality Anti- Aliasing.
 *
 * Polygon Rasterizer that is used to render filled polygons with high-quality Anti-
 * Aliasing. Internally, by default, the class uses integer coordinates in format 24.8,
 * i.e. 24 bits for integer part and 8 bits for fractional - see POLY_BASE_SHIFT. This
 * class can be used in the following  way:
 *
 * 1. setFillingRule(FillingRule fr) - optional.
 *
 * 2. reset()
 *
 * 3. moveTo(x, y) / lineTo(x, y) - make the polygon. One can create
 *    more than one contour, but each contour must consist of at least 3 vertices, i.e.
 *    moveTo(x1, y1); lineTo(x2, y2); lineTo(x3, y3);
 *    is the absolute minimum of vertices that define a triangle. The algorithm does not
 *    check either the number of vertices nor coincidence of their coordinates, but in
 *    the worst case it just won't draw anything. The order of the vertices (clockwise
 *    or counterclockwise)
 *    is important when using the non-zero filling rule (fill_non_zero). In this case
 *    the vertex order of all the contours must be the same if you want your
 *    intersecting polygons to be without "holes". You actually can use different
 *    vertices order. If the contours do not intersect each other the order is not
 *    important anyway. If they do, contours with the same vertex order will be rendered
 *    without "holes" while the intersecting contours with different orders will have
 *    "holes".
 *
 * setFillingRule() can be called anytime before "sweeping".
 */
class Rasterizer
{
public:
    /**
     * Determine the sub pixel accuracy, to be more precise, the number of bits of the
     * fractional part of the coordinates.
     */
    enum
    {
        POLY_BASE_SHIFT = 5,                   ///< Number of bits reserved for fraction part
        POLY_BASE_SIZE = 1 << POLY_BASE_SHIFT, ///< The value to divide or multiply with to convert to / from this format
        POLY_BASE_MASK = POLY_BASE_SIZE - 1    ///< The value used to mask the fraction
    };

    /**
     * Determine the area accuracy, to be more precise, the number of bits of the fractional
     * part of the areas when calculating scanlines.
     */
    enum
    {
        AA_SHIFT = 8,           ///< Number of bits reserved for fraction part when calculating the area
        AA_NUM = 1 << AA_SHIFT, ///< The value to divide or multiply with to convert to / from this format
        AA_MASK = AA_NUM - 1,   ///< The value used to mask the fraction
        AA_2NUM = AA_NUM * 2,   ///< Number of fraction bits when multiplying two area numbers
        AA_2MASK = AA_2NUM - 1  ///< Mask for fraction bits when multiplying two area numbers
    };

    /** Values that represent filling rules. */
    enum FillingRule
    {
        FILL_NON_ZERO, ///< Filling rule to fill anything inside the outmost border of the outline.
        FILL_EVEN_ODD  ///< Filling rule to fill using xor rule inside the outline.
    };

    /** Initializes a new instance of the Rasterizer class. */
    Rasterizer()
        : fillingRule(FILL_NON_ZERO)
    {
    }

    /** Resets this object. Basically this is done by resetting the the Outline. */
    void reset()
    {
        outline.reset();
    }

    /**
     * Sets the filling rule to be used when rendering the outline.
     *
     * @param  fillingRule The filling rule.
     */
    void setFillingRule(FillingRule fillingRule)
    {
        this->fillingRule = fillingRule;
    }

    /**
     * Move to.
     *
     * @param  x The x coordinate.
     * @param  y The y coordinate.
     */
    void moveTo(int x, int y)
    {
#ifndef SIMULATOR
        if (!outline.wasOutlineTooComplex())
#endif
        {
            outline.moveTo(x, y);
        }
    }

    /**
     * Line to.
     *
     * @param  x The x coordinate.
     * @param  y The y coordinate.
     */
    void lineTo(int x, int y)
    {
#ifndef SIMULATOR
        if (!outline.wasOutlineTooComplex())
#endif
        {
            outline.lineTo(x, y);
        }
    }

    /**
     * Calculates the alpha.
     *
     * @param  area The area.
     *
     * @return The calculated alpha.
     */
    unsigned calculateAlpha(int area) const
    {
        int cover = area >> (Rasterizer::POLY_BASE_SHIFT * 2 + 1 - AA_SHIFT);

        if (cover < 0)
        {
            cover = -cover;
        }
        if (fillingRule == FILL_EVEN_ODD)
        {
            cover &= AA_2MASK;
            if (cover > AA_NUM)
            {
                cover = AA_2NUM - cover;
            }
        }
        if (cover > AA_MASK)
        {
            cover = AA_MASK;
        }
        return cover;
    }

    /**
     * Renders this object.
     *
     * @tparam Renderer Type of the renderer.
     * @param [in] r The Renderer to process.
     *
     * @return true there was enough memory available to draw the outline and render the
     *         graphics, false if there was insufficient memory and nothing was drawn.
     */
    template <class Renderer>
    bool render(Renderer& r)
    {
        const Cell* cells = outline.getCells();
        unsigned numCells = outline.getNumCells();
        if (numCells == 0)
        {
            return true;
        }

        // Not enough memory allocated, the outline will look wrong. Do not proceed with the drawing.
        if (outline.wasOutlineTooComplex())
        {
            return false;
        }

        int x, y;
        int cover;
        int alpha;
        int area;

        scanline.reset();

        cover = 0;
        const Cell* curCell = cells++;
        numCells--;
        for (;;)
        {
            const Cell* startCell = curCell;

            int coord = curCell->packedCoord();
            x = curCell->x;
            y = curCell->y;

            area = startCell->area;
            cover += startCell->cover;

            //accumulate all start cells
            while (numCells-- > 0)
            {
                curCell = cells++;
                if (curCell->packedCoord() != coord)
                {
                    break;
                }
                area += curCell->area;
                cover += curCell->cover;
            }

            if (area)
            {
                alpha = calculateAlpha((cover << (Rasterizer::POLY_BASE_SHIFT + 1)) - area);
                if (alpha)
                {
                    if (scanline.isReady(y))
                    {
                        r.render(scanline);
                        scanline.resetSpans();
                    }
                    scanline.addCell(x, y, alpha);
                }
                x++;
            }

            if (numCells == unsigned(-1))
            {
                break;
            }

            if (curCell->x > x)
            {
                alpha = calculateAlpha(cover << (Rasterizer::POLY_BASE_SHIFT + 1));
                if (alpha)
                {
                    if (scanline.isReady(y))
                    {
                        r.render(scanline);
                        scanline.resetSpans();
                    }
                    scanline.addSpan(x, y, curCell->x - x, alpha);
                }
            }
        }

        if (scanline.getNumSpans())
        {
            r.render(scanline);
        }
        return true;
    }

    /**
     * Sets maximum render y coordinate. This is passed to the Outline to avoid registering
     * any Cell that has a y coordinate less than zero of higher than the given y.
     *
     * @param  y The max y coordinate to render for the Outline.
     */
    void setMaxRenderY(int y)
    {
        outline.setMaxRenderY(y);
    }

    /**
     * Determines if we the outline was too complex to draw completely.
     *
     * @return True if it was too complex, false if not.
     */
    bool wasOutlineTooComplex()
    {
        return outline.wasOutlineTooComplex();
    }

private:
    /**
     * Copy constructor.
     *
     * @param  parameter1 The first parameter.
     */
    Rasterizer(const Rasterizer&);

    /**
     * Assignment operator.
     *
     * @param  parameter1 The first parameter.
     *
     * @return A shallow copy of this object.
     */
    const Rasterizer& operator=(const Rasterizer&);

    Outline outline;         ///< The outline
    Scanline scanline;       ///< The scanline
    FillingRule fillingRule; ///< The filling rule
};

} // namespace touchgfx
/// @endcond

#endif // RASTERIZER_HPP
