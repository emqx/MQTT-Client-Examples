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

#include <touchgfx/Math3D.hpp>
#include <touchgfx/TextureMapTypes.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/transforms/DisplayTransformation.hpp>
#include <touchgfx/widgets/TextureMapper.hpp>

namespace touchgfx
{
TextureMapper::TextureMapper()
    : Widget(),
      currentRenderingAlgorithm(NEAREST_NEIGHBOR),
      alpha(255),
      xBitmapPosition(0.0f),
      yBitmapPosition(0.0f),
      xAngle(0.0f),
      yAngle(0.0f),
      zAngle(0.0f),
      scale(1.0f),
      xOrigo(0.0f),
      yOrigo(0.0f),
      zOrigo(1000.0f),
      xCamera(0.0f),
      yCamera(0.0f),
      cameraDistance(1000.0f),
      imageX0(0.0f),
      imageY0(0.0f),
      imageZ0(1.0f),
      imageX1(0.0f),
      imageY1(0.0f),
      imageZ1(1.0f),
      imageX2(0.0f),
      imageY2(0.0f),
      imageZ2(1.0f),
      imageX3(0.0f),
      imageY3(0.0f),
      imageZ3(1.0f),
      subDivisionSize(12)
{
}

void TextureMapper::setBitmap(const Bitmap& bmp)
{
    bitmap = bmp;
    setWidth(bitmap.getWidth());
    setHeight(bitmap.getHeight());
    applyTransformation();
}

void TextureMapper::applyTransformation()
{
    const uint8_t n = 4;

    int imgWidth = Bitmap(bitmap).getWidth() + 1;
    int imgHeight = Bitmap(bitmap).getHeight() + 1;

    touchgfx::Point4 vertices[n] =
    {
        touchgfx::Point4(xBitmapPosition - 1, yBitmapPosition - 1, cameraDistance),
        touchgfx::Point4(xBitmapPosition - 1 + imgWidth, yBitmapPosition - 1, cameraDistance),
        touchgfx::Point4(xBitmapPosition - 1 + imgWidth, yBitmapPosition - 1 + imgHeight, cameraDistance),
        touchgfx::Point4(xBitmapPosition - 1, yBitmapPosition - 1 + imgHeight, cameraDistance),
    };
    touchgfx::Point4 transformed[n];

    Vector4 center(xOrigo, yOrigo, zOrigo);

    Matrix4x4 translateToCenter;
    translateToCenter.concatenateXTranslation(-center.getX()).concatenateYTranslation(-center.getY()).concatenateZTranslation(-center.getZ());

    Matrix4x4 rotateAroundCenter;
    rotateAroundCenter.concatenateXRotation(xAngle).concatenateYRotation(yAngle).concatenateZRotation(zAngle);

    Matrix4x4 scaleAroundCenter;
    scaleAroundCenter.concatenateXScale(scale).concatenateYScale(scale).concatenateZScale(scale);

    Matrix4x4 translateFromCenter;
    translateFromCenter.concatenateXTranslation(center.getX()).concatenateYTranslation(center.getY()).concatenateZTranslation(center.getZ());

    Matrix4x4 transform = translateFromCenter * scaleAroundCenter * rotateAroundCenter * translateToCenter;

    Matrix4x4 translateToCamera;
    translateToCamera.concatenateXTranslation(-xCamera);
    translateToCamera.concatenateYTranslation(-yCamera);

    Matrix4x4 perspectiveProject;
    perspectiveProject.setViewDistance(cameraDistance);

    Matrix4x4 translateFromCamera;
    translateFromCamera.concatenateXTranslation(xCamera).concatenateYTranslation(yCamera);

    transform = translateFromCamera * perspectiveProject * translateToCamera * transform;

    for (int i = 0; i < n; i++)
    {
        transformed[i] = transform * vertices[i];
    }

    imageX0 = ((float)transformed[0].getX() * cameraDistance / (float)transformed[0].getZ());
    imageY0 = ((float)transformed[0].getY() * cameraDistance / (float)transformed[0].getZ());
    imageZ0 = ((float)transformed[0].getZ());

    imageX1 = ((float)transformed[1].getX() * cameraDistance / (float)transformed[1].getZ());
    imageY1 = ((float)transformed[1].getY() * cameraDistance / (float)transformed[1].getZ());
    imageZ1 = ((float)transformed[1].getZ());

    imageX2 = ((float)transformed[2].getX() * cameraDistance / (float)transformed[2].getZ());
    imageY2 = ((float)transformed[2].getY() * cameraDistance / (float)transformed[2].getZ());
    imageZ2 = ((float)transformed[2].getZ());

    imageX3 = ((float)transformed[3].getX() * cameraDistance / (float)transformed[3].getZ());
    imageY3 = ((float)transformed[3].getY() * cameraDistance / (float)transformed[3].getZ());
    imageZ3 = ((float)transformed[3].getZ());
}

Rect TextureMapper::getBoundingRect() const
{
    // MIN and MAX are macros so do not insert them into each other
    float minXf = MIN(imageX0, imageX1);
    minXf = MIN(minXf, imageX2);
    minXf = floorf(MIN(minXf, imageX3));
    int16_t minX = (int16_t)(MAX(0, minXf));

    float maxXf = MAX(imageX0, imageX1);
    maxXf = MAX(maxXf, imageX2);
    maxXf = ceilf(MAX(maxXf, imageX3));
    int16_t maxX = getWidth();
    maxX = (int16_t)(MIN(maxX, maxXf));

    float minYf = MIN(imageY0, imageY1);
    minYf = MIN(minYf, imageY2);
    minYf = floorf(MIN(minYf, imageY3));
    int16_t minY = (int16_t)(MAX(0, minYf));

    float maxYf = MAX(imageY0, imageY1);
    maxYf = MAX(maxYf, imageY2);
    maxYf = ceilf(MAX(maxYf, imageY3));
    int16_t maxY = getHeight();
    maxY = (int16_t)(MIN(maxY, maxYf));

    return Rect(minX, minY, maxX - minX, maxY - minY);
}

void TextureMapper::updateAngles(float newXAngle, float newYAngle, float newZAngle)
{
    Rect rBefore = getBoundingRect();

    this->xAngle = newXAngle;
    this->yAngle = newYAngle;
    this->zAngle = newZAngle;

    applyTransformation();

    Rect rAfter = getBoundingRect();
    rAfter.expandToFit(rBefore);
    invalidateRect(rAfter);
}

void TextureMapper::setScale(float _scale)
{
    this->scale = _scale;

    applyTransformation();
}

void TextureMapper::draw(const Rect& invalidatedArea) const
{
    if (!alpha)
    {
        return;
    }
    uint16_t* fb = HAL::getInstance()->lockFrameBuffer();

    // Setup texture coordinates
    float right = (float)(bitmap.getWidth());
    float bottom = (float)(bitmap.getHeight());
    float textureU0 = -1.0f;
    float textureV0 = -1.0f;
    float textureU1 = right;
    float textureV1 = -1.0f;
    float textureU2 = right;
    float textureV2 = bottom;
    float textureU3 = -1.0f;
    float textureV3 = bottom;
    if (HAL::DISPLAY_ROTATION == rotate90)
    {
        textureU0 = -1.0f;
        textureV0 = right;
        textureU1 = -1.0f;
        textureV1 = -1.0f;
        textureU2 = bottom;
        textureV2 = -1.0f;
        textureU3 = bottom;
        textureV3 = right;
    }

    float triangleXs[3];
    float triangleYs[3];
    float triangleZs[3];
    float triangleUs[3];
    float triangleVs[3];

    // Determine winding order
    Vector4 zeroToOne(imageX1 - imageX0, imageY1 - imageY0, imageZ1 - imageZ0);
    Vector4 zeroToTwo(imageX2 - imageX0, imageY2 - imageY0, imageZ2 - imageZ0);
    Vector4 normal = zeroToOne.crossProduct(zeroToTwo);

    if (normal.getZ() > 0)
    {
        triangleXs[0] = imageX0;
        triangleXs[1] = imageX1;
        triangleXs[2] = imageX2;
        triangleYs[0] = imageY0;
        triangleYs[1] = imageY1;
        triangleYs[2] = imageY2;
        triangleZs[0] = imageZ0;
        triangleZs[1] = imageZ1;
        triangleZs[2] = imageZ2;

        triangleUs[0] = textureU0;
        triangleUs[1] = textureU1;
        triangleUs[2] = textureU2;
        triangleVs[0] = textureV0;
        triangleVs[1] = textureV1;
        triangleVs[2] = textureV2;
    }
    else
    {
        // invert due to the triangles winding order (showing backface of the triangle)
        triangleXs[1] = imageX0;
        triangleXs[0] = imageX1;
        triangleXs[2] = imageX2;
        triangleYs[1] = imageY0;
        triangleYs[0] = imageY1;
        triangleYs[2] = imageY2;
        triangleZs[1] = imageZ0;
        triangleZs[0] = imageZ1;
        triangleZs[2] = imageZ2;

        triangleUs[1] = textureU0;
        triangleUs[0] = textureU1;
        triangleUs[2] = textureU2;
        triangleVs[1] = textureV0;
        triangleVs[0] = textureV1;
        triangleVs[2] = textureV2;
    }

    drawTriangle(invalidatedArea, fb, triangleXs, triangleYs, triangleZs, triangleUs, triangleVs);

    if (normal.getZ() > 0)
    {
        triangleXs[0] = imageX0;
        triangleXs[1] = imageX2;
        triangleXs[2] = imageX3;
        triangleYs[0] = imageY0;
        triangleYs[1] = imageY2;
        triangleYs[2] = imageY3;
        triangleZs[0] = imageZ0;
        triangleZs[1] = imageZ2;
        triangleZs[2] = imageZ3;

        triangleUs[0] = textureU0;
        triangleUs[1] = textureU2;
        triangleUs[2] = textureU3;
        triangleVs[0] = textureV0;
        triangleVs[1] = textureV2;
        triangleVs[2] = textureV3;
    }
    else
    {
        triangleXs[1] = imageX0;
        triangleXs[0] = imageX2;
        triangleXs[2] = imageX3;
        triangleYs[1] = imageY0;
        triangleYs[0] = imageY2;
        triangleYs[2] = imageY3;
        triangleZs[1] = imageZ0;
        triangleZs[0] = imageZ2;
        triangleZs[2] = imageZ3;

        triangleUs[1] = textureU0;
        triangleUs[0] = textureU2;
        triangleUs[2] = textureU3;
        triangleVs[1] = textureV0;
        triangleVs[0] = textureV2;
        triangleVs[2] = textureV3;
    }

    drawTriangle(invalidatedArea, fb, triangleXs, triangleYs, triangleZs, triangleUs, triangleVs);

    HAL::getInstance()->unlockFrameBuffer();
}

void TextureMapper::drawTriangle(const Rect& invalidatedArea, uint16_t* fb, const float* triangleXs, const float* triangleYs, const float* triangleZs, const float* triangleUs, const float* triangleVs) const
{
    // Area to redraw. Relative to the TextureMapper.
    Rect dirtyArea = Rect(0, 0, getWidth(), getHeight()) & invalidatedArea;

    // Absolute position of the TextureMapper.
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
    float y0 = triangleYs[0]; //lint !e578
    float y1 = triangleYs[1]; //lint !e578
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

    uint16_t subDivs = subDivisionSize;
    if (point0.Z == point1.Z && point1.Z == point2.Z)
    {
        subDivs = 0xFFFF; // Max: One sweep
    }
    HAL::lcd().drawTextureMapTriangle(dest, vertices, src, absoluteRect, dirtyAreaAbsolute, lookupRenderVariant(), alpha, subDivs);
}

RenderingVariant TextureMapper::lookupRenderVariant() const
{
    RenderingVariant renderVariant;
    if (currentRenderingAlgorithm == NEAREST_NEIGHBOR)
    {
        renderVariant = lookupNearestNeighborRenderVariant(bitmap);
    }
    else
    {
        renderVariant = lookupBilinearRenderVariant(bitmap);
    }
    return renderVariant;
}

Rect TextureMapper::getSolidRect() const
{
    return Rect(0, 0, 0, 0);
}
} // namespace touchgfx
