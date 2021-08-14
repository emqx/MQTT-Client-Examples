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
 * @file touchgfx/Bitmap.hpp
 *
 * Declares the touchgfx::Bitmap class.
 */
#ifndef BITMAP_HPP
#define BITMAP_HPP

#include <cassert>
#include <touchgfx/hal/Types.hpp>

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION < 6000000)
// Keil5 compiler issues irrelevant warning relating to missing ctor initialization for BitmapData.
#pragma diag_suppress 368
#endif

namespace touchgfx
{
/**
 * This type shall be used by the application to define unique IDs for all bitmaps in the
 * system. The application shall define bitmap IDs in the range [0, number of bitmaps -
 * 1].
 */
typedef uint16_t BitmapId;

const BitmapId BITMAP_ANIMATION_STORAGE = 0xFFFEU; ///< A virtual id representing animation storage.
const BitmapId BITMAP_INVALID = 0xFFFFU;           ///< Define the bitmapId of an invalid bitmap

/**
 * This class provides a proxy object for a bitmap image stored in the application specific
 * bitmap database. The proxy provides access to the raw bitmap data as well as metadata.
 */
class Bitmap
{
public:
    /** Color data of a clut can be stored in the following formats. */
    enum ClutFormat
    {
        CLUT_FORMAT_L8_ARGB8888, ///< 32-bit, 8 bits for each of red, green, blue and alpha
        CLUT_FORMAT_L8_RGB888,   ///< 24-bit, 8 bits for each of red, green and blue. No per pixel alpha channel
        CLUT_FORMAT_L8_RGB565    ///< 16-bit, 5 bits for red, 6 bits for green, 5 bits for blue. No per pixel alpha channel
    };

    /** Data of a bitmap can be stored in the following formats. */
    enum BitmapFormat
    {
        RGB565,   ///< 16-bit, 5 bits for red, 6 bits for green, 5 bits for blue. No alpha channel
        RGB888,   ///< 24-bit, 8 bits for each of red, green and blue. No alpha channel
        ARGB8888, ///< 32-bit, 8 bits for each of red, green, blue and alpha channel
        BW,       ///< 1-bit, black / white. No alpha channel
        BW_RLE,   ///< 1-bit, black / white. No alpha channel. Image is compressed with horizontal RLE
        GRAY2,    ///< 2-bit grayscale
        GRAY4,    ///< 4-bit grayscale
        ARGB2222, ///< 8-bit color
        ABGR2222, ///< 8-bit color
        RGBA2222, ///< 8-bit color
        BGRA2222, ///< 8-bit color
        L8,       ///< 8-bit indexed color
        A4        ///< 4-bit alpha level
    };

    /** Data of a bitmap. */
    struct BitmapData
    {
        const uint8_t* const data;            ///< The data of this Bitmap
        const uint8_t* const extraData;       ///< The data of either the alpha channel (if present) or clut data in case of indexed color bitmap. 0 if not used
        const uint16_t width;                 ///< The width of the Bitmap
        const uint16_t height;                ///< The height of the Bitmap
        const uint16_t solidRect_x;           ///< The x coordinate of the maximum solid rectangle of the Bitmap
        const uint16_t solidRect_y;           ///< The y coordinate of the maximum solid rectangle of the Bitmap
        const uint16_t solidRect_width : 13;  ///< The width of the maximum solid rectangle of the Bitmap
        const uint16_t format_hi : 3;         ///< Determine the format of the data (high 3 bits)
        const uint16_t solidRect_height : 13; ///< The height of the maximum solid rectangle of the Bitmap
        const uint16_t format_lo : 3;         ///< Determine the format of the data (low 3 bits)

        /**
         * Gets the Bitmap format by combining the high and low parts (format_hi << 3) | format_lo.
         *
         * @return The BitmapFormat
         */
        BitmapFormat getFormat() const
        {
            return (BitmapFormat)((format_hi << 3) | format_lo);
        }
    };

    /** Data of a dynamic Bitmap. */
    struct DynamicBitmapData
    {
        Rect solid;         ///< The solidRect of this Bitmap
        uint16_t width;     ///< The width of the Bitmap
        uint16_t height;    ///< The height of the Bitmap
        uint8_t format : 5; ///< Determine the format of the data
        uint8_t inuse : 1;  ///< Zero if not in use
        uint8_t extra : 2;  ///< Extra data field, depending on format
    };

    /** Cache bookkeeping. */
    struct CacheTableEntry
    {
        uint8_t* data; ///< Pointer to location of image data for this Bitmap in the cache. 0 if bitmap not cached.
    };

    /**
     * Creates and binds a Bitmap instance to the corresponding entry in the BitmapData
     * array.
     *
     * @param  id (Optional) The unique bitmap identifier.
     */
    explicit Bitmap(const BitmapId id = BITMAP_INVALID)
        : bitmapId(id)
    {
    }

    /**
     * Gets the id of this Bitmap.
     *
     * @return The id of this Bitmap.
     */
    BitmapId getId() const
    {
        assert(bitmaps != 0 && "Bitmap database has not been initialized.");
        return bitmapId;
    }

    /**
     * Gets a pointer to the Bitmap data.
     *
     * @return A pointer to the raw Bitmap data.
     *
     * @note If this Bitmap is cached, it will return the cached version of Bitmap data.
     */
    const uint8_t* getData() const;

    ///@cond
    /**
     * Gets a pointer to the alpha/extra data.
     *
     * @return A pointer to the extra data
     *
     * @see getExtraData
     *
     * @note If this Bitmap is cached, it will return the cached version of alpha data for this
     *       Bitmap.
     *
     * @deprecated Use Bitmap::getExtraData().
     */
    TOUCHGFX_DEPRECATED(
        "Use Bitmap::getExtraData().",
        const uint8_t* getAlphaData() const);
    ///@endcond

    /**
     * Gets a pointer to the extra (alpha) data, if present in the Bitmap. For images stored
     * in L8 format, a pointer to the CLUT will be returned. For non-opaque RGB565 images, a
     * pointer to the alpha channel will be returned.
     *
     * @return A pointer to the raw alpha channel data or CLUT. If no alpha channel or CLUT
     *         exist for the given Bitmap, 0 is returned.
     *
     * @note If this Bitmap is cached, it will return the cached version of alpha data for this
     *       Bitmap.
     */
    const uint8_t* getExtraData() const;

    /**
     * Gets the format of how the Bitmap is stored.
     *
     * @return The format of how the Bitmap data is stored.
     */
    BitmapFormat getFormat() const;

    /**
     * Gets the width of the Bitmap in pixels.
     *
     * @return The Bitmap width in pixels.
     */
    uint16_t getWidth() const;

    /**
     * Gets the height of the Bitmap in pixels.
     *
     * @return The Bitmap height in pixels.
     */
    uint16_t getHeight() const;

    /**
     * Gets the rectangle describing the dimensions of the Bitmap.
     *
     * @return a Rect describing the dimensions of this Bitmap.
     */
    Rect getRect() const
    {
        return Rect(0, 0, getWidth(), getHeight());
    }

    /**
     * Query if this object has an alpha channel.
     *
     * @return True if the bitmap contains an alpha channel (an alpha value for each pixel)
     */
    bool isAlphaPerPixel() const
    {
        assert(bitmaps != 0 && "Bitmap database has not been initialized.");
        if (getFormat() == L8)
        {
            return false; // No Alpha channel for indexed L8 bitmaps
        }
        return ((bitmaps != 0) && (bitmapId < numberOfBitmaps)) ? (bitmaps[bitmapId].extraData != 0) : false;
    }

    /**
     * Gets the largest solid, i.e. not transparent, rectangle in the Bitmap.
     *
     * @return The maximum solid rectangle of the Bitmap.
     */
    Rect getSolidRect() const;

    /**
     * Query if this object has transparent pixels.
     *
     * @return True if this bitmap has transparent pixels.
     */
    bool hasTransparentPixels() const;

    /**
     * Registers an array of bitmaps. All Bitmap instances are bound to this database. This
     * function is called automatically from HAL::touchgfx_generic_init().
     *
     * @param      data                   A reference to the BitmapData storage array.
     * @param      n                      The number of bitmaps in the array.
     * @param [in] cachep                 (Optional) Pointer to memory region in which bitmap
     *                                    data can be cached.
     * @param      csize                  (Optional) Size of cache memory region in bytes (0 if
     *                                    unused)
     * @param      numberOfDynamicBitmaps (Optional) Number of dynamic bitmaps to be allowed in
     *                                    the cache.
     */
    static void registerBitmapDatabase(const BitmapData* data, const uint16_t n, uint16_t* cachep = 0, uint32_t csize = 0,
                                       uint32_t numberOfDynamicBitmaps = 0);

    /**
     * Cache this Bitmap into unused RAM in the bitmap cache. A memory region large enough
     * to hold this bitmap must be configured and a large enough part of it must be
     * available. Caching of a bitmap may involve a defragmentation of the bitmap cache.
     *
     * @param  id The id of the Bitmap to cache.
     *
     * @return true if caching went well, false otherwise.
     *
     * @see registerBitmapDatabase, compactCache
     */
    static bool cache(BitmapId id);

    /**
     * Replace a Bitmap in RAM with another Bitmap. The Bitmaps must have same size.
     *
     * @param  out The id of the Bitmap to remove from the cache.
     * @param  in  The id of the Bitmap to cache.
     *
     * @return true if the replacement went well, false otherwise.
     */
    static bool cacheReplaceBitmap(BitmapId out, BitmapId in);

    /**
     * Remove this Bitmap from the RAM cache. Unless the Bitmap is
     * otherwise stored in (slower) memory it can not be drawn anymore
     * and must be cached again before use. The RAM freed can be used
     * for caching of another bitmap.
     *
     * @param  id The id of the Bitmap to cache.
     *
     * @return true if Bitmap was found and removed, false otherwise.
     *
     * @see registerBitmapDatabase
     */
    static bool cacheRemoveBitmap(BitmapId id);

    /**
     * Get address of cache buffer for this Bitmap.
     *
     * @param  id The id of the Bitmap in cache.
     *
     * @return Address if Bitmap was found, zero otherwise.
     *
     * @note The address is only valid until next Bitmap::cache() call.
     */
    static uint8_t* cacheGetAddress(BitmapId id);

    /**
     * Check if the Bitmap is cached.
     *
     * @param  id The id of the Bitmap.
     *
     * @return true if Bitmap is cached.
     */
    static bool cacheIsCached(BitmapId id);

    /**
     * Cache all bitmaps from the Bitmap Database into RAM. A memory region large enough to
     * hold all bitmaps must be configured.
     *
     * @return True if all bitmaps where cached.
     *
     * @see cache
     */
    static bool cacheAll();

    /** Clears the cached bitmaps from RAM. */
    static void clearCache();

    /**
     * Create a dynamic Bitmap. The clutFormat parameter is ignored for bitmaps not in L8
     * format. Creation of a new dynamic bitmap may cause existing dynamic bitmaps to be
     * moved in memory. Do not rely on bitmap memory addresses of dynamic bitmaps obtained
     * from dynamicBitmapGetAddress() to be valid across calls to dynamicBitmapCreate().
     *
     * @param  width      Width of the Bitmap.
     * @param  height     Height of the Bitmap.
     * @param  format     Bitmap format of the Bitmap.
     * @param  clutFormat (Optional) Color lookup table format of the Bitmap.
     *
     * @return BitmapId of the new Bitmap or #BITMAP_INVALID if cache memory is full.
     *
     * @see DynamicBitmapData
     */
    static BitmapId dynamicBitmapCreate(const uint16_t width, const uint16_t height, BitmapFormat format, ClutFormat clutFormat = CLUT_FORMAT_L8_ARGB8888);

    /**
     * Delete a dynamic Bitmap.
     *
     * @param  id The BitmapId of the dynamic Bitmap.
     *
     * @return true if it succeeds, false if it fails.
     */
    static bool dynamicBitmapDelete(BitmapId id);

    /**
     * Get the address of the dynamic Bitmap data. It is important that the address of a
     * dynamic Bitmap is not stored elsewhere as a dynamic Bitmap may be moved in memory
     * when other bitmaps are added and removed. Only store the BitmapId and ask for the
     * address of the Bitmap data when needed. The address of a dynamic bitmap may change
     * when other dynamic bitmaps are added and removed.
     *
     * @param  id The BitmapId of the dynamic bitmap.
     *
     * @return null if it fails, else an uint8_t*.
     *
     * @note Never keep the address of dynamic images, only store the BitmapId as that will not
     *       change.
     */
    static uint8_t* dynamicBitmapGetAddress(BitmapId id);

    /**
     * Set the solid rectangle of a dynamic Bitmap. Only relevant for ARGB8888 Bitmap and
     * 8bpp Bitmap formats, as these formats include an alpha channel. The solid part of the
     * Bitmap is drawn faster than the transparent parts.
     *
     * @param  id        The identifier.
     * @param  solidRect The solid rectangle.
     *
     * @return true if it succeeds, false if it fails.
     */
    static bool dynamicBitmapSetSolidRect(BitmapId id, const Rect& solidRect);

    /**
     * Updates the solid rectangle of a dynamic Bitmap to include the given rectangle. Only
     * relevant for ARGB8888 bitmap and 8bpp bitmap formats, as these formats include an
     * alpha channel. The solid part of the Bitmap is drawn faster than the transparent
     * parts.
     *
     * @param  id        The identifier.
     * @param  solidRect The solid rectangle.
     *
     * @return true if it succeeds, false if it fails.
     */
    static bool dynamicBitmapAddSolidRect(BitmapId id, const Rect& solidRect);

    ///@cond
    /**
     * Returns the number of dynamic bitmaps created.
     *
     * @return The number of dynamic bitmaps.
     */
    static uint32_t dynamicBitmapGetNumberOfBitmaps();

    /**
     * Equality operator.
     *
     * @param  other The bitmap to compare with.
     *
     * @return True if this bitmap has the same id as the other bitmap.
     *
     * @deprecated Simply compare the BitmapId of the bitmaps using Bitmap::getId().
     */
    TOUCHGFX_DEPRECATED(
        "Simply compare the BitmapId of the bitmaps using Bitmap::getId().",
        bool operator==(const Bitmap& other) const)
    {
        return bitmapId == other.bitmapId;
    }
    ///@endcond

    ///@cond
    /**
     * Inequality operator.
     *
     * @param  other The bitmap to compare with.
     *
     * @return True if this bitmap has a different id than the other bitmap.
     *
     * @deprecated Simply compare the BitmapId of the bitmaps using Bitmap::getId().
     */
    TOUCHGFX_DEPRECATED(
        "Simply compare the BitmapId of the bitmaps using Bitmap::getId().",
        bool operator!=(const Bitmap& other) const)
    {
        return bitmapId != other.bitmapId;
    }
    ///@endcond

    /**
     * Register a memory region in which Bitmap data can be cached.
     *
     * @param [in] cachep                 Pointer to memory region in which bitmap data can be
     *                                    cached.
     * @param      csize                  Size of cache memory region in bytes.
     * @param      numberOfDynamicBitmaps (Optional) Number of dynamic bitmaps to be allowed in
     *                                    the cache.
     */
    static void setCache(uint16_t* cachep, uint32_t csize, uint32_t numberOfDynamicBitmaps = 0);

    /**
     * Removes the Bitmap cache. The memory can hereafter be used for other purposes. All
     * dynamic Bitmap IDs are invalid after this.
     */
    static void removeCache();

    /**
     * Gets the address of the first unused memory in the cache. Can be used in advanced
     * application to reduce power consumption of external RAM by turning off unused RAM.
     *
     * @return Returns the highest used address in the cache.
     */
    static uint8_t* getCacheTopAddress()
    {
        return nextFreeData;
    }

    /**
     * Compact the bitmap cache to get continuous free memory on top. This method is called
     * by Bitmap::cache when required.
     */
    static void compactCache();

private:
    static uint32_t getSizeOfBitmap(BitmapId id);

    static bool copyBitmapToCache(BitmapId id, uint8_t* const dst);

    BitmapId bitmapId;
    static const BitmapData* bitmaps;
    static DynamicBitmapData* dynBitmaps;
    static CacheTableEntry* cacheTable; ///< Address of allocation point cache
    static BitmapId* allocationTable;   ///< Order of allocations in cache
    static uint8_t* nextFreeData;
    static uint16_t nextAllocationIndex;
    static uint32_t memoryRemaining;
    static uint32_t totalMemory;
    static uint16_t numberOfBitmaps;
    static uint16_t numberOfDynamicBitmaps;
    static uint16_t uncachedCount; ///< Uncached images, sort of ...
};

} // namespace touchgfx

#endif // BITMAP_HPP
