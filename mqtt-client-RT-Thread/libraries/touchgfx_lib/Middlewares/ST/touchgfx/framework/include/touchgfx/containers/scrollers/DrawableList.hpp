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
 * @file touchgfx/containers/scrollers/DrawableList.hpp
 *
 * Declares the touchgfx::DrawableListItemsInterface (abstract), touchgfx::DrawableListItems
 * and touchgfx::DrawableList classes.
 */
#ifndef DRAWABLELIST_HPP
#define DRAWABLELIST_HPP

#include <touchgfx/containers/Container.hpp>

namespace touchgfx
{
/**
 * A drawable list items interface. Used to pass the allocated array of drawable elements to
 * ScrollList::setDrawables(), ScrollWheel::setDrawables() or
 * ScrollWheelWithSelectionStyle::setDrawables(). Provides easy access to each element
 * in the array as well as the size of the array.
 *
 * @see ScrollList::setDrawables, ScrollWheel::setDrawables,
 *      ScrollWheelWithSelectionStyle::setDrawables
 */
class DrawableListItemsInterface
{
public:
    /** Finalizes an instance of the DrawableListItemsInterface class. */
    virtual ~DrawableListItemsInterface()
    {
    }

    /**
     * Gets a drawable at a given index.
     *
     * @param  index Zero-based index of the drawable.
     *
     * @return Null if it fails, else the drawable.
     */
    virtual Drawable* getDrawable(int16_t index) = 0;

    /**
     * Gets number of drawables.
     *
     * @return The number of drawables.
     */
    virtual int16_t getNumberOfDrawables() = 0;
};

/**
 * An array of drawables used by DrawableList. This class is used to ease the setup of
 * a callback function to get access to a specific drawable in the array.
 *
 * Example usage:
 * @code
 *       static const int NUMBER_OF_DRAWABLES = 5;
 *       DrawableListItems<TextAreaWithOneWildcardContainer, NUMBER_OF_DRAWABLES> menuItems;
 * @endcode
 *
 * @tparam TYPE Type of the drawables. Can be a simple drawable, such as Image or a more
 *              complex container.
 * @tparam SIZE Size of the array. This is the number of drawables to allocate and should be
 *              all visible drawables on the screen at any given time.
 */
template <class TYPE, int SIZE>
class DrawableListItems : public DrawableListItemsInterface
{
public:
    virtual Drawable* getDrawable(int16_t index)
    {
        assert(index >= 0 && index < SIZE);
        return &element[index];
    }

    /**
     * Array indexer operator.
     *
     * @param  index Zero-based index of elements to access.
     *
     * @return The indexed value.
     */
    TYPE& operator[](int index)
    {
        assert(index >= 0 && index < SIZE);
        return element[index];
    }

    virtual int16_t getNumberOfDrawables()
    {
        return SIZE;
    }

    TYPE element[SIZE]; ///< The array of drawables
};

/**
 * A container able to display many items using only a few drawables. This is done by only
 * having drawables for visible items, and populating these drawables with new content
 * when each of these become visible.
 *
 * This means that all drawables must have an identical structure in some way, for
 * example an Image or a Container with a button and a text.
 */
class DrawableList : public Container
{
public:
    DrawableList();

    /**
     * @copydoc Container::setWidth
     *
     * @note If the list is vertical, the width is also propagated to all drawables in the list.
     */
    virtual void setWidth(int16_t width);

    /**
     * @copydoc Container::setHeight
     *
     * @note If the list is horizontal, the height is also propagated to all drawables in the list.
     */
    virtual void setHeight(int16_t height);

    /**
     * Sets a horizontal or vertical layout. If parameter horizontal is set true, all
     * drawables are arranged side by side. If horizontal is set false, the drawables are
     * arranged above and below each other (vertically).
     *
     * @param  horizontal True to align drawables horizontal, false to align drawables
     *                    vertically.
     *
     * @see ScrollBase::setHorizontal, getHorizontal
     *
     * @note Default value is false, i.e. vertical layout.
     */
    virtual void setHorizontal(bool horizontal);

    /**
     * Gets the orientation of the drawables, previously set using setHorizontal().
     *
     * @return True if it horizontal, false if it is vertical.
     *
     * @see ScrollBase::getHorizontal, setHorizontal
     */
    virtual bool getHorizontal() const;

    /**
     * Sets whether the list is circular (infinite) or not. A circular list is a list where
     * the first drawable re-appears after the last item in the list - and the last item in
     * the list appears before the first item in the list.
     *
     * @param  circular True if the list should be circular, false if the list should not be
     *                  circular.
     *
     * @see ScrollBase::setCircular, getCircular
     */
    virtual void setCircular(bool circular);

    /**
     * Gets the circular setting, previously set using setCircular().
     *
     * @return True if the list is circular (infinite), false if the list is not circular
     *         (finite).
     *
     * @see ScrollBase::getCircular, setCircular
     */
    virtual bool getCircular() const;

    /**
     * Sets drawables size. The drawable is is the size of each drawable in the list in the
     * set direction of the list (this is enforced by the DrawableList class). The specified
     * margin is added above and below each item for spacing. The entire size of an item is
     * thus size + 2 * spacing.
     *
     * For a horizontal list each element will be \a drawableSize high and have the same
     * width as set using setWidth(). For a vertical list each element will be \a
     * drawableSize wide and have the same height as set using setHeight().
     *
     * @param  drawableSize   The size of the drawable.
     * @param  drawableMargin The margin around drawables (margin before and margin after).
     *
     * @see setWidth, setHeight, setHorizontal
     */
    void setDrawableSize(int16_t drawableSize, int16_t drawableMargin);

    /**
     * Gets size of each item. This equals the drawable size plus the drawable margin as set
     * in setDrawables(). Equals getDrawableSize() + 2 * getDrawableMargin().
     *
     * @return The item size.
     *
     * @see setDrawables, setDrawableSize, getDrawableMargin
     *
     * @note Not the same as getDrawableSize().
     */
    virtual int16_t getItemSize() const;

    /**
     * Gets drawable size as set by setDrawables.
     *
     * @return The drawable size.
     *
     * @see setDrawables
     */
    virtual int16_t getDrawableSize() const;

    /**
     * Gets drawable margin.
     *
     * Gets drawable margin as set by setDrawables.
     *
     * @return The drawable margin.
     */
    virtual int16_t getDrawableMargin() const;

    /**
     * Sets the drawables parameters. These parameters are \li The access class to the array
     * of drawables \li The offset in the drawableListItems array to start using drawable
     * and \li Callback to update the contents of a drawable.
     *
     * @param [in] drawableListItems       Number of drawables allocated.
     * @param [in] drawableItemIndexOffset The offset of the drawable item.
     * @param [in] updateDrawableCallback  A callback to update the contents of a drawable.
     *
     * @see getRequiredNumberOfDrawables
     */
    virtual void setDrawables(DrawableListItemsInterface& drawableListItems,
                              int16_t drawableItemIndexOffset,
                              GenericCallback<DrawableListItemsInterface*, int16_t, int16_t>& updateDrawableCallback);

    /**
     * Gets number of drawables based on the size of each item and the size of the widget.
     *
     * @return The number of drawables.
     *
     * @see setDrawables
     */
    int16_t getNumberOfDrawables() const;

    /**
     * Sets number of items in the list. This forces all drawables to be updated to ensure
     * that the content is correct.
     *
     * @param  numberOfItems Number of items.
     *
     * @note The DrawableList is refreshed to reflect the change.
     */
    void setNumberOfItems(int16_t numberOfItems);

    /**
     * Gets number of items in the DrawableList, as previously set using setNumberOfItems().
     *
     * @return The number of items.
     *
     * @see setNumberOfItems
     */
    int16_t getNumberOfItems() const;

    /**
     * Gets required number of drawables. After setting up the DrawableList it is possible
     * to request how many drawables are needed to ensure that the list can always be drawn
     * properly. If the DrawableList has been setup with fewer Drawables than the required
     * number of drawables, part of the lower part of the DrawableList will look wrong.
     *
     * The number of required drawables depend on the size of the widget and the size of the
     * drawables and the margin around drawables. If there are fewer drawables than required,
     * the widget will not display correctly. If there are more drawables than required,
     * some will be left unused.
     *
     * @return The required number of drawables.
     *
     * @see setDrawables
     */
    int16_t getRequiredNumberOfDrawables() const;

    /**
     * Sets virtual coordinate. Does not move to the given coordinate, but places the
     * drawables and fill correct content into the drawables to give the impression that
     * everything has been scrolled to the given coordinate.
     *
     * Setting a value of 0 means that item 0 is at the start of the DrawableList. Setting a
     * value of "-getItemSize()" places item 0 outside the start of the DrawableList and
     * item 1 at the start of it.
     *
     * Items that are completely outside of view, will be updated with new content using the
     * provided callback from setDrawables(). Care is taken to not fill drawables more than
     * strictly required.
     *
     * @param  ofs The virtual coordinate.
     *
     * @see getOffset, setDrawables
     */
    void setOffset(int32_t ofs);

    /**
     * Gets offset, as previously set using setOffset().
     *
     * @return The virtual offset.
     *
     * @see setOffset
     */
    int32_t getOffset() const;

    /**
     * Gets item stored in a given Drawable.
     *
     * @param  drawableIndex Zero-based index of the drawable.
     *
     * @return The item index.
     */
    int16_t getItemIndex(int16_t drawableIndex) const;

    /**
     * Gets drawable indices. Useful when the number of items is smaller than the number of
     * drawables as the same item might be in more than one drawable on the screen (if the
     * DrawableList is circular). The passed array will be filled with the drawable indices
     * and the number of indices found is returned.
     *
     * @param       itemIndex          Zero-based index of the item.
     * @param [out] drawableIndexArray Array where the drawable indices are stored.
     * @param       arraySize          Size of drawable array.
     *
     * @return The number of drawable indices found.
     *
     * @see getItemIndex, setCircular, getDrawableIndex
     */
    int16_t getDrawableIndices(int16_t itemIndex, int16_t* drawableIndexArray, int16_t arraySize) const;

    /**
     * Gets the drawable index of an item. If the number of items is smaller than the number
     * of drawables and the DrawableList is circular, the same item can be in more than one
     * drawable. In that case, calling this function again with the previously returned
     * index as second parameter, the index of the next drawable containing the item will be
     * returned.
     *
     * @param  itemIndex         Index of the item.
     * @param  prevDrawableIndex (Optional) Index of the previous drawable. If given, search
     *                           starts after this index.
     *
     * @return The first drawable index with the given item. Returns -1 if the item is not
     *         in a drawable.
     *
     * @see getDrawableIndices
     */
    int16_t getDrawableIndex(int16_t itemIndex, int16_t prevDrawableIndex = -1) const;

    /**
     * Refresh drawables. Useful to call if the number or items, their size or other
     * properties have changed.
     */
    void refreshDrawables();

    /**
     * Item changed.
     *
     * Item changed and drawables containing this item must be updated. This function can be
     * called when an item has changed and needs to be updated on screen. If the given item
     * is displayed on screen, possible more than once for cyclic lists, each drawable is
     * request to refresh its content to reflect the new value.
     *
     * @param  itemIndex Zero-based index of the item.
     */
    void itemChanged(int16_t itemIndex);

private:
    bool isHorizontal;         ///< True if list is horizontal, false if not
    bool isCircular;           ///< True if list is circular, false if not
    int32_t offset;            ///< The offset of item 0
    int16_t itemSize;          ///< Size of each item (including margin)
    int16_t itemMargin;        ///< The margin around each item (included in itemSize)
    int16_t numItems;          ///< Number of items
    int16_t numDrawables;      ///< Number of drawables
    int16_t firstItem;         ///< The first visible item
    int16_t firstDrawable;     ///< The first visible drawable
    bool drawablesInitialized; ///< True if all drawables initialized

    int16_t firstDrawableIndex;                                                     ///< The offset when accessing DrawableListItems
    DrawableListItemsInterface* drawableItems;                                      ///< The drawable items
    GenericCallback<DrawableListItemsInterface*, int16_t, int16_t>* updateDrawable; ///< The update drawable callback
};

} // namespace touchgfx

#endif // DRAWABLELIST_HPP
