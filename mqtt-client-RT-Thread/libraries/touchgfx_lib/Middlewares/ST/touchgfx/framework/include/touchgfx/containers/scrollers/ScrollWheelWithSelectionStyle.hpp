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
 * @file touchgfx/containers/scrollers/ScrollWheelWithSelectionStyle.hpp
 *
 * Declares the touchgfx::ScrollWheelWithSelectionStyle class.
 */
#ifndef SCROLLWHEELWITHSELECTIONSTYLE_HPP
#define SCROLLWHEELWITHSELECTIONSTYLE_HPP

#include <touchgfx/containers/scrollers/DrawableList.hpp>
#include <touchgfx/containers/scrollers/ScrollWheelBase.hpp>

namespace touchgfx
{
/**
 * A scroll wheel is very much like the digit selector on a padlock with numbers. The digits
 * always snap into place and exactly one number is always the "selected" number.
 * Similar to an ordinary ScrollWheel, but with a different style for the selected item
 * which can thus be bold, have a different color or similar effect to highlight it and
 * help create a 3D effect using very few resources.
 *
 * @see DrawableList, ScrollWheel
 */
class ScrollWheelWithSelectionStyle : public ScrollWheelBase
{
public:
    ScrollWheelWithSelectionStyle();

    virtual void setWidth(int16_t width);

    virtual void setHeight(int16_t height);

    virtual void setHorizontal(bool horizontal);

    virtual void setCircular(bool circular);

    virtual void setNumberOfItems(int16_t numberOfItems);

    virtual void setSelectedItemOffset(int16_t offset);

    /**
     * Sets selected item extra size to make the size of the area for the center drawables
     * larger.
     *
     * @param  extraSizeBefore The extra size before.
     * @param  extraSizeAfter  The extra size after.
     *
     * @see setSelectedItemOffset
     */
    virtual void setSelectedItemExtraSize(int16_t extraSizeBefore, int16_t extraSizeAfter);

    /**
     * Gets selected item extra size before.
     *
     * @return The selected item extra size before.
     *
     * @see setSelectedItemExtraSize
     */
    virtual int16_t getSelectedItemExtraSizeBefore() const;

    /**
     * Gets selected item extra size after.
     *
     * @return The selected item extra size after.
     *
     * @see setSelectedItemExtraSize
     */
    virtual int16_t getSelectedItemExtraSizeAfter() const;

    /**
     * Sets margin around selected item. This like an invisible area added before and after
     * the selected item (including extra size).
     *
     * @param  marginBefore The margin before.
     * @param  marginAfter  The margin after.
     *
     * @see setSelectedItemOffset, setSelectedItemExtraSize
     */
    virtual void setSelectedItemMargin(int16_t marginBefore, int16_t marginAfter);

    /**
     * Gets selected item margin before.
     *
     * @return The selected item margin before.
     *
     * @see setSelectedItemMargin
     */
    virtual int16_t getSelectedItemMarginBefore() const;

    /**
     * Gets selected item margin after.
     *
     * @return The selected item margin after.
     *
     * @see setSelectedItemMargin
     */
    virtual int16_t getSelectedItemMarginAfter() const;

    /**
     * Sets the selected item offset. This is the distance from the beginning of the
     * ScrollWheel measured in pixels. The distance before and after that should also be
     * drawn using the center drawables - for example to extend area of emphasized elements -
     * can also be specified. Further, if a gap is needed between the "normal" drawables and
     * the center drawables - for example to give the illusion that that items disappear
     * under a graphical element, only to appear in the center.
     *
     * This is a combination of setSelectedItemOffset, setSelectedItemExtraSize and
     * setSelectedItemMargin.
     *
     * @param  offset          The offset of the selected item.
     * @param  extraSizeBefore The extra size before the selected item.
     * @param  extraSizeAfter  The extra size after the selected item.
     * @param  marginBefore    The margin before the selected item.
     * @param  marginAfter     The margin after the selected item.
     *
     * @see setSelectedItemOffset, setSelectedItemExtraSize, setSelectedItemMargin
     */
    virtual void setSelectedItemPosition(int16_t offset, int16_t extraSizeBefore, int16_t extraSizeAfter, int16_t marginBefore, int16_t marginAfter);

    /**
     * @copydoc ScrollWheelBase::setDrawableSize
     */
    virtual void setDrawableSize(int16_t drawableSize, int16_t drawableMargin);

    /**
     * Setups the widget. Numerous parameters control the position of the widget, the two
     * scroll lists inside and the values in them.
     *
     * @param [in] drawableListItems            Number of drawables in outer array.
     * @param [in] updateDrawableCallback       The callback to update a drawable.
     * @param [in] centerDrawableListItems      Number of drawables in center array.
     * @param [in] updateCenterDrawableCallback The callback to update a center drawable.
     */
    virtual void setDrawables(DrawableListItemsInterface& drawableListItems, GenericCallback<DrawableListItemsInterface*, int16_t, int16_t>& updateDrawableCallback,
                              DrawableListItemsInterface& centerDrawableListItems, GenericCallback<DrawableListItemsInterface*, int16_t, int16_t>& updateCenterDrawableCallback);

    virtual void itemChanged(int itemIndex);

    virtual void initialize()
    {
        ScrollWheelBase::initialize();
        list1.refreshDrawables();
        list2.refreshDrawables();
    }

protected:
    virtual void setOffset(int32_t offset);

    /**
     * Refresh drawable lists layout. Ensure that the three DrawableLists are places
     * correctly and setup properly. This is typically done after the
     * ScrollWheelWithSelectionStyle has been resized or the size of the selected item is
     * changed.
     */
    void refreshDrawableListsLayout();

    int16_t drawablesInFirstList;        ///< List of drawables in firsts
    DrawableList list1;                  ///< The center list
    DrawableList list2;                  ///< The list for items not in the center
    int16_t extraSizeBeforeSelectedItem; ///< The distance before selected item
    int16_t extraSizeAfterSelectedItem;  ///< The distance after selected item
    int16_t marginBeforeSelectedItem;    ///< The distance before selected item
    int16_t marginAfterSelectedItem;     ///< The distance after selected item

    DrawableListItemsInterface* drawables;       ///< The drawables at the beginning and end of the scroll wheel
    DrawableListItemsInterface* centerDrawables; ///< The drawables at the center of the scroll wheel

    GenericCallback<DrawableListItemsInterface*, int16_t, int16_t>* originalUpdateDrawableCallback;       ///< The original update drawable callback
    GenericCallback<DrawableListItemsInterface*, int16_t, int16_t>* originalUpdateCenterDrawableCallback; ///< The original update center drawable callback
};

} // namespace touchgfx

#endif // SCROLLWHEELWITHSELECTIONSTYLE_HPP
