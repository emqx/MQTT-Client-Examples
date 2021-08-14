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
 * @file touchgfx/Screen.hpp
 *
 * Declares the touchgfx::Screen class.
 */
#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <touchgfx/Application.hpp>
#include <touchgfx/JSMOCHelper.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/events/ClickEvent.hpp>
#include <touchgfx/events/DragEvent.hpp>
#include <touchgfx/events/GestureEvent.hpp>
#include <touchgfx/lcd/LCD.hpp>

namespace touchgfx
{
class Drawable;
class Transition;

/**
 * A Screen represents a full-screen drawable area. Applications create specific screens by
 * subclassing this class. Each Screen has a root container to which drawables are
 * added. The Screen makes sure to delegate draw requests and various events to the
 * appropriate drawables in correct order.
 */
class Screen
{
public:
    /** Initializes a new instance of the Screen class. */
    Screen();

    /** Finalizes an instance of the Screen class. */
    virtual ~Screen()
    {
    }

    /**
     * Tells the screen to draw its entire area.
     *
     * @note The more specific draw(Rect&amp;) version is preferred when possible.
     */
    void draw();

    /**
     * Starts a JSMOC run, analyzing what parts of what widgets should be redrawn.
     *
     * @param [in] invalidatedArea The area to redraw, expressed in absolute coordinates.
     *
     * @note SMOC is an abbreviation of <em>S&oslash;ren &amp; Martin's Occlusion Culling</em>.
     */
    void startSMOC(const Rect& invalidatedArea);

    /**
     * Recursive JSMOC function. This is the actual occlusion culling implementation.
     *
     * @param [in] invalidatedArea The area to redraw, expressed in absolute coordinates.
     * @param [in] widgetToDraw    Widget currently being drawn.
     *
     * @note JSMOC is an abbreviation of <em>Jesper, S&oslash;ren &amp; Martin's Occlusion Culling</em>.
     */
    void JSMOC(const Rect& invalidatedArea, Drawable* widgetToDraw);

    /**
     * Tell the screen to draw the specified area. Will traverse the drawables tree from in
     * z-order and delegate draw to them.
     *
     * @param [in] rect The area in absolute coordinates.
     *
     * @note The given rect must be in absolute coordinates.
     */
    virtual void draw(Rect& rect);

    /**
     * Called by Application::switchScreen() when this screen is going to be displayed. Base
     * version does nothing, but place any screen specific initialization code in an
     * overridden version.
     *
     * @see Application::switchScreen
     */
    virtual void setupScreen()
    {
    }

    /**
     * Called by Application::handleTickEvent() when the transition to the screen is done. Base
     * version does nothing, but override to do screen specific initialization code that has
     * to be done after the transition to the screen.
     *
     * @see Application::handleTickEvent
     */
    virtual void afterTransition()
    {
    }

    /**
     * Called by Application::switchScreen() when this screen will no longer be displayed.
     * Base version does nothing, but place any screen specific cleanup code in an
     * overridden version.
     *
     * @see touchgfx::Application::switchScreen
     */
    virtual void tearDownScreen()
    {
    }

    /**
     * Traverse the drawables in reverse z-order and notify them of a click event.
     *
     * @param  evt The event to handle.
     */
    virtual void handleClickEvent(const ClickEvent& evt);

    /**
     * Traverse the drawables in reverse z-order and notify them of a drag event.
     *
     * @param  evt The event to handle.
     */
    virtual void handleDragEvent(const DragEvent& evt);

    /**
     * Handle gestures. Traverses drawables in reverse-z and notifies them of the gesture.
     *
     * @param  evt The event to handle.
     */
    virtual void handleGestureEvent(const GestureEvent& evt);

    /**
     * Called by the Application on the current screen with a frequency of
     * Application::TICK_INTERVAL_MS.
     */
    virtual void handleTickEvent()
    {
    }

    /**
     * Called by the Application on the reception of a "key", the meaning of which is
     * platform/application specific. Default implementation does nothing.
     *
     * @param  key The key to handle.
     */
    virtual void handleKeyEvent(uint8_t key)
    {
    }

    /**
     * Determines if using JSMOC.
     *
     * @return true if this screen uses the JSMOC drawing algorithm.
     */
    bool usingSMOC() const
    {
        return useSMOC;
    }

    /**
     * Enables the transition to access the containers.
     *
     * @param [in] trans The transition to bind.
     */
    void bindTransition(Transition& trans);

    /**
     * Obtain a reference to the root container of this screen.
     *
     * @return The root container.
     */
    Container& getRootContainer()
    {
        return container;
    }

protected:
    /**
     * Determines whether to use JSMOC or painter's algorithm for drawing.
     *
     * @param  enabled true if JSMOC should be enabled, false if disabled (meaning painter's
     *                 algorithm is employed instead).
     */
    void useSMOCDrawing(bool enabled);

    /**
     * Add a drawable to the content container.
     *
     * @param [in] d The Drawable to add.
     *
     * @note Must not be called with a Drawable that was already added to the screen. If in doubt,
     *       call remove() first.
     */
    void add(Drawable& d)
    {
        container.add(d);
    }

    /**
     * Removes a drawable from the content container. Safe to call even if the drawable was
     * never added (in which case nothing happens).
     *
     * @param [in] d The Drawable to remove.
     */
    void remove(Drawable& d)
    {
        container.remove(d);
    }

    Container container; ///< The container contains the contents of the screen.

protected:
    Drawable* focus; ///< The drawable currently in focus (set when DOWN_PRESSED is received).

private:
    int16_t fingerAdjustmentX;
    int16_t fingerAdjustmentY;
    bool useSMOC;
};

} // namespace touchgfx

#endif // SCREEN_HPP
