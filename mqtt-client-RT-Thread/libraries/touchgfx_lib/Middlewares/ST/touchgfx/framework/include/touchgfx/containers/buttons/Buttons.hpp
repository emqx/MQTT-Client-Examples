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
 * @file touchgfx/containers/buttons/Buttons.hpp
 *
 * Declares the various FlexButton styles by combining often used template classes.
 */
#ifndef BUTTONS_HPP
#define BUTTONS_HPP

#include <touchgfx/containers/buttons/ClickButtonTrigger.hpp>
#include <touchgfx/containers/buttons/RepeatButtonTrigger.hpp>
#include <touchgfx/containers/buttons/ToggleButtonTrigger.hpp>
#include <touchgfx/containers/buttons/TouchButtonTrigger.hpp>

#include <touchgfx/containers/buttons/AnimatedImageButtonStyle.hpp>
#include <touchgfx/containers/buttons/BoxWithBorderButtonStyle.hpp>
#include <touchgfx/containers/buttons/IconButtonStyle.hpp>
#include <touchgfx/containers/buttons/ImageButtonStyle.hpp>
#include <touchgfx/containers/buttons/TextButtonStyle.hpp>
#include <touchgfx/containers/buttons/TiledImageButtonStyle.hpp>
#include <touchgfx/containers/buttons/TwoWildcardTextButtonStyle.hpp>
#include <touchgfx/containers/buttons/WildcardTextButtonStyle.hpp>

namespace touchgfx
{
/** Defines an alias representing the box click button. */
typedef BoxWithBorderButtonStyle<ClickButtonTrigger> BoxClickButton;

/** Defines an alias representing the box repeat button. */
typedef BoxWithBorderButtonStyle<RepeatButtonTrigger> BoxRepeatButton;

/** Defines an alias representing the box toggle button. */
typedef BoxWithBorderButtonStyle<ToggleButtonTrigger> BoxToggleButton;

/** Defines an alias representing the box touch button. */
typedef BoxWithBorderButtonStyle<TouchButtonTrigger> BoxTouchButton;

/** Defines an alias representing the image click button. */
typedef ImageButtonStyle<ClickButtonTrigger> ImageClickButton;

/** Defines an alias representing the image repeat button. */
typedef ImageButtonStyle<RepeatButtonTrigger> ImageRepeatButton;

/** Defines an alias representing the image touch button. */
typedef ImageButtonStyle<TouchButtonTrigger> ImageTouchButton;

/** Defines an alias representing the image toggle button. */
typedef ImageButtonStyle<ToggleButtonTrigger> ImageToggleButton;

/** Defines an alias representing the icon click button. */
typedef IconButtonStyle<ClickButtonTrigger> IconClickButton;

/** Defines an alias representing the icon repeat button. */
typedef IconButtonStyle<RepeatButtonTrigger> IconRepeatButton;

/** Defines an alias representing the icon touch button. */
typedef IconButtonStyle<TouchButtonTrigger> IconTouchButton;

/** Defines an alias representing the icon toggle button. */
typedef IconButtonStyle<ToggleButtonTrigger> IconToggleButton;

/** Defines an alias representing the icon image click button. */
typedef ImageButtonStyle<IconButtonStyle<ClickButtonTrigger> > IconImageClickButton;

/** Defines an alias representing the icon image repeat button. */
typedef ImageButtonStyle<IconButtonStyle<RepeatButtonTrigger> > IconImageRepeatButton;

/** Defines an alias representing the icon image touch button. */
typedef ImageButtonStyle<IconButtonStyle<TouchButtonTrigger> > IconImageTouchButton;

/** Defines an alias representing the icon image toggle button. */
typedef ImageButtonStyle<IconButtonStyle<ToggleButtonTrigger> > IconImageToggleButton;

/** Defines an alias representing the text click button. */
typedef TextButtonStyle<ClickButtonTrigger> TextClickButton;

/** Defines an alias representing the text repeat button. */
typedef TextButtonStyle<RepeatButtonTrigger> TextRepeatButton;

/** Defines an alias representing the text touch button. */
typedef TextButtonStyle<TouchButtonTrigger> TextTouchButton;

/** Defines an alias representing the text toggle button. */
typedef TextButtonStyle<ToggleButtonTrigger> TextToggleButton;

/** Defines an alias representing the tiled image click button. */
typedef TiledImageButtonStyle<ClickButtonTrigger> TiledImageClickButton;

/** Defines an alias representing the tiled image repeat button. */
typedef TiledImageButtonStyle<RepeatButtonTrigger> TiledImageRepeatButton;

/** Defines an alias representing the tiled image touch button. */
typedef TiledImageButtonStyle<TouchButtonTrigger> TiledImageTouchButton;

/** Defines an alias representing the tiled image toggle button. */
typedef TiledImageButtonStyle<ToggleButtonTrigger> TiledImageToggleButton;

/** Defines an alias representing the wildcard text click button. */
typedef WildcardTextButtonStyle<ClickButtonTrigger> WildcardTextClickButton;

/** Defines an alias representing the wildcard text repeat button. */
typedef WildcardTextButtonStyle<RepeatButtonTrigger> WildcardTextRepeatButton;

/** Defines an alias representing the wildcard text touch button. */
typedef WildcardTextButtonStyle<TouchButtonTrigger> WildcardTextTouchButton;

/** Defines an alias representing the wildcard text toggle button. */
typedef WildcardTextButtonStyle<ToggleButtonTrigger> WildcardTextToggleButton;

/** Defines an alias representing the wildcard text click button. */
typedef TwoWildcardTextButtonStyle<ClickButtonTrigger> TwoWildcardTextClickButton;

/** Defines an alias representing the wildcard text repeat button. */
typedef TwoWildcardTextButtonStyle<RepeatButtonTrigger> TwoWildcardTextRepeatButton;

/** Defines an alias representing the wildcard text touch button. */
typedef TwoWildcardTextButtonStyle<TouchButtonTrigger> TwoWildcardTextTouchButton;

/** Defines an alias representing the wildcard text toggle button. */
typedef TwoWildcardTextButtonStyle<ToggleButtonTrigger> TwoWildcardTextToggleButton;

/** Defines an alias representing the animated image click button. */
typedef AnimatedImageButtonStyle<ClickButtonTrigger> AnimatedImageClickButton;

/** Defines an alias representing the animated image repeat button. */
typedef AnimatedImageButtonStyle<RepeatButtonTrigger> AnimatedImageRepeatButton;

/** Defines an alias representing the animated image touch button. */
typedef AnimatedImageButtonStyle<TouchButtonTrigger> AnimatedImageTouchButton;

/** Defines an alias representing the animated image toggle button. */
typedef AnimatedImageButtonStyle<ToggleButtonTrigger> AnimatedImageToggleButton;

} // namespace touchgfx

#endif // BUTTONS_HPP
