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

#include <touchgfx/containers/scrollers/ScrollWheel.hpp>

namespace touchgfx
{
void ScrollWheel::setDrawables(DrawableListItemsInterface& drawableListItems, GenericCallback<DrawableListItemsInterface*, int16_t, int16_t>& updateDrawableCallback)
{
    stopAnimation();
    numberOfDrawables = drawableListItems.getNumberOfDrawables();

    list.setDrawables(drawableListItems, 0, updateDrawableCallback);

    setOffset(0);
}
} // namespace touchgfx
