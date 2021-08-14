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
 * @file touchgfx/canvas_widget_renderer/RenderingBuffer.hpp
 *
 * Declares the touchgfx::RenderingBuffer class. Used internally by CanvasWidgetRenderer.
 */
#ifndef RENDERINGBUFFER_HPP
#define RENDERINGBUFFER_HPP

/// @cond
namespace touchgfx
{
/**
 * Rendering buffer wrapper. This class does not know anything about memory organizations, all
 * it does it keeps an array of pointers to each pixel row. The general rules of
 * rendering are as follows.
 *
 * 1. Allocate or create somehow a rendering buffer itself. Since
 *    the library does not depend on any particular platform or architecture it was
 *    decided that it's your responsibility to create and destroy rendering buffers
 *    properly. You can use any available mechanism to create it - you can use a system
 *    API function, simple memory allocation, or even statically defined array. You also
 *    should know the memory organization (or possible variants)
 *    in your system. For example, there's an R,G,B or B,G,R organizations with one byte
 *    per component (three bytes per pixel) is used very often. So, if you intend to use
 *    class render_bgr24, for example, you should allocate at least width*height*3 bytes
 *    of memory.
 *
 * 2. Create a RenderingBuffer object and then call method attach(). It requires
 *    a pointer to the buffer itself, width and height of the buffer in pixels, and the
 *    length of the row in bytes. All these values must properly correspond to the
 *    memory organization. The argument stride is used because in reality the row length
 *    in bytes does not obligatory correspond with the width of the image in pixels,
 *    i.e. it cannot be simply calculated as width_in_pixels * bytes_per_pixel. For
 *    example, it must be aligned to 4 bytes in Windows bitmaps. Method attach() can be
 *    called more than once. The execution time of it is very little, still it allocates
 *    memory of heigh * sizeof(char*) bytes and has a loop while (height--) {...}, so
 *    it's unreasonable to call it every time before drawing any single pixel :-)
 *
 * 3. Create an object (or a number of objects) of a rendering class, such as
 *    renderer_bgr24_solid, renderer_bgr24_image and so on. These classes require a
 *    pointer to the RenderingBuffer object, but they do not perform any considerable
 *    operations except storing this pointer. So, rendering objects can be created on
 *    demand almost any time. These objects know about concrete memory organization
 *    (this knowledge is hard coded), so actually, the memory you allocated or created
 *    in clause 1 should actually be in correspondence to the needs of the rendering
 *    class.
 *
 * 4. Render your image using rendering classes, for example, Rasterizer
 *
 * 5. Display the result, or store it, or whatever. It's also your
 *    responsibility and depends on the platform.
 */
class RenderingBuffer
{
public:
    /** Initializes a new instance of the RenderingBuffer class. */
    RenderingBuffer();

    /** Finalizes an instance of the RenderingBuffer class. */
    virtual ~RenderingBuffer();

    /**
     * Initializes a new instance of the RenderingBuffer class.
     *
     * @param [in] buf_     Pointer to the framebuffer where the image is rendered.
     * @param      xAdjust_ Horizontal adjustment of the x coordinate, used when bits per pixel
     *                      is less than eight which implies that a uint8_t pointer
     *                      cannot precisely address the start of the framebuffer.
     * @param      width_   The width of the framebuffer to write.
     * @param      height_  The height of the framebuffer to write.
     * @param      stride_  How much to add the a pointer inside the framebuffer to advance to
     *                      the next line in the framebuffer.
     */
    RenderingBuffer(unsigned char* buf_,
                    unsigned char xAdjust_,
                    unsigned width_,
                    unsigned height_,
                    int stride_);

    /**
     * Attaches a buffer. Can be used if the buffer is not ready when the Rendering buffer
     * is created initially.
     *
     * @param [in] buf_     Pointer to the framebuffer where the image is rendered.
     * @param      xAdjust_ Horizontal adjustment of the x coordinate, used when bits per pixel
     *                      is less than eight which implies that a uint8_t pointer
     *                      cannot precisely address the start of the framebuffer.
     * @param      width_   The width of the framebuffer to write.
     * @param      height_  The height of the framebuffer to write.
     * @param      stride_  How much to add the a pointer inside the framebuffer to advance to
     *                      the next line in the framebuffer.
     */
    void attach(unsigned char* buf_,
                unsigned char xAdjust_,
                unsigned width_,
                unsigned height_,
                int stride_);

    /**
     * Gets x coordinate adjust.
     *
     * @return The x coordinate adjust.
     */
    unsigned char getXAdjust() const
    {
        return xAdjust;
    }

    /**
     * Gets the width.
     *
     * @return The width.
     */
    unsigned getWidth() const
    {
        return width;
    }

    /**
     * Gets the height.
     *
     * @return The height.
     */
    unsigned getHeight() const
    {
        return height;
    }

    /**
     * Tests if a given coordinate is inside the RenderingBuffer.
     *
     * @param  x The x coordinate.
     * @param  y The y coordinate.
     *
     * @return true if (x,y) is inside the RenderingBuffer, false otherwise.
     */
    bool inbox(int x, int y) const
    {
        return x >= 0 && y >= 0 && x < int(width) && y < int(height);
    }

    /**
     * Gets a pointer to the given row in the RenderingBuffer.
     *
     * @param  y The line number, ie the row.
     *
     * @return The pointer to the start of the given line in the RenderingBuffer.
     */
    unsigned char* row(unsigned y)
    {
        return buf + stride * y;
    }

    /**
     * Gets a pointer to the given row in the RenderingBuffer.
     *
     * @param  y The line number, ie the row.
     *
     * @return The pointer to the start of the given line in the RenderingBuffer.
     */
    const unsigned char* row(unsigned y) const
    {
        return buf + stride * y;
    }

private:
    RenderingBuffer(const RenderingBuffer&);

    unsigned char* buf;
    unsigned char xAdjust;
    unsigned width;
    unsigned height;
    int stride;
};

} // namespace touchgfx
/// @endcond

#endif // RENDERINGBUFFER_HPP
