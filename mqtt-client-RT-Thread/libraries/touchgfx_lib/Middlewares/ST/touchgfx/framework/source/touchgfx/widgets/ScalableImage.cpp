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

#include <touchgfx/TextureMapTypes.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/transforms/DisplayTransformation.hpp>
#include <touchgfx/widgets/ScalableImage.hpp>

namespace touchgfx
{
ScalableImage::ScalableImage()
    : Widget(),
      currentScalingAlgorithm(BILINEAR_INTERPOLATION),
      alpha(255)
{
}

void ScalableImage::setBitmap(const Bitmap& bmp)
{
    bitmap = bmp;
    setWidth(bitmap.getWidth());
    setHeight(bitmap.getHeight());
}

void ScalableImage::setScalingAlgorithm(ScalingAlgorithm algorithm)
{
    currentScalingAlgorithm = algorithm;
}

ScalableImage::ScalingAlgorithm ScalableImage::getScalingAlgorithm()
{
    return currentScalingAlgorithm;
}

void ScalableImage::drawTriangle(const Rect& invalidatedArea, uint16_t* fb, const float* triangleXs, const float* triangleYs, const float* triangleZs, const float* triangleUs, const float* triangleVs) const
{
    // Area to redraw. Relative to the scalableImage.
    Rect dirtyArea = Rect(0, 0, getWidth(), getHeight()) & invalidatedArea;

    // Absolute position of the scalableImage.
    Rect dirtyAreaAbsolute = dirtyArea;
    translateRectToAbsolute(dirtyAreaAbsolute);

    Rect absoluteRect = getAbsoluteRect();
    DisplayTransformation::transformDisplayToFrameBuffer(absoluteRect);

    // Transform rects to match framebuffer coordinates
    // This is needed if the display is rotated compared to the framebuffer
    DisplayTransformation::transformDisplayToFrameBuffer(dirtyArea, this->getRect());
    DisplayTransformation::transformDisplayToFrameBuffer(dirtyAreaAbsolute);

    // Get a pointer to the bitmap data, return if no bitmap found
    const uint16_t* textmap = (const uint16_t*)bitmap.getData();
    if (!textmap)
    {
        return;
    }

    float x0 = triangleXs[0];
    float x1 = triangleXs[1];
    float x2 = triangleXs[2];
    float y0 = triangleYs[0];
    float y1 = triangleYs[1];
    float y2 = triangleYs[2];

    DisplayTransformation::transformDisplayToFrameBuffer(x0, y0, this->getRect());
    DisplayTransformation::transformDisplayToFrameBuffer(x1, y1, this->getRect());
    DisplayTransformation::transformDisplayToFrameBuffer(x2, y2, this->getRect());

    Point3D vertices[3];
    Point3D point0 = { floatToFixed28_4(x0), floatToFixed28_4(y0), (float)(triangleZs[0]), (float)(triangleUs[0]), (float)(triangleVs[0]) };
    Point3D point1 = { floatToFixed28_4(x1), floatToFixed28_4(y1), (float)(triangleZs[1]), (float)(triangleUs[1]), (float)(triangleVs[1]) };
    Point3D point2 = { floatToFixed28_4(x2), floatToFixed28_4(y2), (float)(triangleZs[2]), (float)(triangleUs[2]), (float)(triangleVs[2]) };

    vertices[0] = point0;
    vertices[1] = point1;
    vertices[2] = point2;

    DrawingSurface dest = { fb, HAL::FRAME_BUFFER_WIDTH };
    TextureSurface src = { textmap, bitmap.getExtraData(), bitmap.getWidth(), bitmap.getHeight(), bitmap.getWidth() };

    HAL::lcd().drawTextureMapTriangle(dest, vertices, src, absoluteRect, dirtyAreaAbsolute, lookupRenderVariant(), alpha, 0xFFFF);
}

RenderingVariant ScalableImage::lookupRenderVariant() const
{
    RenderingVariant renderVariant;
    if (currentScalingAlgorithm == NEAREST_NEIGHBOR)
    {
        renderVariant = lookupNearestNeighborRenderVariant(bitmap);
    }
    else
    {
        renderVariant = lookupBilinearRenderVariant(bitmap);
    }
    return renderVariant;
}

void ScalableImage::draw(const Rect& invalidatedArea) const
{
    if (!alpha)
    {
        return;
    }
    uint16_t* fb = HAL::getInstance()->lockFrameBuffer();

    float triangleXs[3];
    float triangleYs[3];
    float triangleZs[3];
    float triangleUs[3];
    float triangleVs[3];

    float imageX0 = 0;
    float imageY0 = 0;
    float imageX1 = imageX0 + getWidth();
    float imageY1 = imageY0;
    float imageX2 = imageX1;
    float imageY2 = imageY0 + getHeight();
    float imageX3 = imageX0;
    float imageY3 = imageY2;

    triangleZs[0] = 100.f;
    triangleZs[1] = 100.f;
    triangleZs[2] = 100.f;

    // Setup texture coordinates
    float right = (float)(bitmap.getWidth());
    float bottom = (float)(bitmap.getHeight());
    float textureU0 = 0.0f;
    float textureV0 = 0.0f;
    float textureU1 = right;
    float textureV1 = 0.0f;
    float textureU2 = right;
    float textureV2 = bottom;
    float textureU3 = 0.0f;
    float textureV3 = bottom;
    if (HAL::DISPLAY_ROTATION == rotate90)
    {
        textureU0 = 0.0f;
        textureV0 = right;
        textureU1 = 0.0f;
        textureV1 = 0.0f;
        textureU2 = bottom;
        textureV2 = 0.0f;
        textureU3 = bottom;
        textureV3 = right;
    }

    triangleXs[0] = imageX0;
    triangleXs[1] = imageX1;
    triangleXs[2] = imageX2;
    triangleYs[0] = imageY0;
    triangleYs[1] = imageY1;
    triangleYs[2] = imageY2;

    triangleUs[0] = textureU0;
    triangleUs[1] = textureU1;
    triangleUs[2] = textureU2;
    triangleVs[0] = textureV0;
    triangleVs[1] = textureV1;
    triangleVs[2] = textureV2;

    drawTriangle(invalidatedArea, fb, triangleXs, triangleYs, triangleZs, triangleUs, triangleVs);

    triangleXs[0] = imageX0;
    triangleXs[1] = imageX2;
    triangleXs[2] = imageX3;
    triangleYs[0] = imageY0;
    triangleYs[1] = imageY2;
    triangleYs[2] = imageY3;

    triangleUs[0] = textureU0;
    triangleUs[1] = textureU2;
    triangleUs[2] = textureU3;
    triangleVs[0] = textureV0;
    triangleVs[1] = textureV2;
    triangleVs[2] = textureV3;

    drawTriangle(invalidatedArea, fb, triangleXs, triangleYs, triangleZs, triangleUs, triangleVs);

    HAL::getInstance()->unlockFrameBuffer();
}

Rect ScalableImage::getSolidRect() const
{
    if (alpha < 255)
    {
        return Rect(0, 0, 0, 0);
    }

    // If original image is completely solid the scaled image will also be
    if ((bitmap.getSolidRect().width == bitmap.getWidth()) && (bitmap.getSolidRect().height == bitmap.getHeight()))
    {
        return bitmap.getSolidRect();
    }
    return Rect(0, 0, 0, 0);
}
} // namespace touchgfx
