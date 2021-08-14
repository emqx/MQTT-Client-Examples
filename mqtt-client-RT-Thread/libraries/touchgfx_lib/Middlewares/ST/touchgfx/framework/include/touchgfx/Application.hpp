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
 * @file touchgfx/Application.hpp
 *
 * Declares the application class.
 */
#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <touchgfx/UIEventListener.hpp>
#include <touchgfx/events/ClickEvent.hpp>
#include <touchgfx/events/DragEvent.hpp>
#include <touchgfx/events/GestureEvent.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/lcd/LCD.hpp>

namespace touchgfx
{
class Drawable;
class Screen;
class Transition;

/**
 * The Application class is the main interface for manipulating screen contents. It holds a
 * pointer to the currently displayed Screen, and delegates draw requests and events to
 * that Screen. Additionally it contains some global application settings.
 */
class Application : public UIEventListener
{
public:
    /**
     * Gets the single instance application.
     *
     * @return The instance of this application.
     */
    static Application* getInstance();

    /**
     * Gets the current screen.
     *
     * @return The current screen.
     */
    Screen* getCurrentScreen()
    {
        return currentScreen;
    }

    /**
     * Switch to another Screen. Will call tearDownScreen on current Screen before switching,
     * and subsequently call setupScreen and draw automatically for the new Screen.
     *
     * @param [in] newScreen A pointer to the new screen.
     */
    virtual void switchScreen(Screen* newScreen);

    /**
     * An application specific function for switching screen. Overloading this can provide a
     * means to switch screen from places that does not have access to a pointer to the new
     * screen. Base implementation is empty.
     *
     * @param screenId An id that maps to the desired screen.
     */
    virtual void appSwitchScreen(uint8_t screenId)
    {
    }

    /** An application specific function for requesting redraw of entire screen. */
    virtual void requestRedraw();

    /**
     * An application specific function for requesting redraw of given Rect.
     *
     * @param [in,out] rect The Rect that must be redrawn.
     */
    virtual void requestRedraw(Rect& rect)
    {
        redraw = rect;
    };

    /**
     * Initiate a draw operation of the entire screen. Standard implementation is to
     * delegate draw request to the current Screen.
     *
     * @deprecated Use draw(Rect&)
     */
    virtual void draw();

    /**
     * Initiate a draw operation of the specified region of the screen. Standard
     * implementation is to delegate draw request to the current Screen.
     *
     * @param [in] rect The area to draw.
     *
     * @note Unlike Widget::draw this is safe to call from user code as it will properly traverse
     *       widgets in z-order.
     * @note The coordinates given must be absolute coordinates.
     */
    virtual void draw(Rect& rect);

    /**
     * Handle a click event. Standard implementation is to delegate the event to the current
     * screen. Called by the framework when a click is detected by some platform specific
     * means.
     *
     * @param evt The ClickEvent.
     */
    virtual void handleClickEvent(const ClickEvent& evt);

    /**
     * Handle drag events. Called by the framework when a drag is detected by some platform
     * specific means. Standard implementation is to delegate drag event to current screen.
     *
     * @param evt The drag event, expressed in absolute coordinates.
     */
    virtual void handleDragEvent(const DragEvent& evt);

    /**
     * Handle gestures. Called by the framework when a gesture is detected by some platform
     * specific means. Standard implementation is to delegate drag event to current screen.
     *
     * @param evt The gesture event.
     */
    virtual void handleGestureEvent(const GestureEvent& evt);

    /**
     * Handle tick. Standard implementation is to delegate tick to the widgets that have
     * registered to receive one. Called by some platform specific means.
     */
    virtual void handleTickEvent();

    /**
     * Handle an incoming character received by the HAL layer. Standard implementation
     * delegates to current screen (which, in turn, does nothing).
     *
     * @param  c The incomming character to handle.
     */
    virtual void handleKeyEvent(uint8_t c);

    /**
     * Evaluates the pending Callback instances. If a callback is valid, it is executed and
     * a Screen transition is executed. This base implementation is empty and does nothing.
     */
    virtual void handlePendingScreenTransition();

    /**
     * This functions allows for deferring draw operations to a later time. If active, calls
     * to draw will simply note that the specified area is dirty, but not perform any actual
     * drawing. When disabling the draw cache, the dirty area will be flushed (drawn)
     * immediately.
     *
     * @param  enableCache if true, all future draw operations will be cached. If false draw
     *                     caching is disabled, and the current cache (if not empty) is
     *                     drawn immediately.
     */
    virtual void cacheDrawOperations(bool enableCache);

    /**
     * Adds a widget to the list of widgets receiving ticks every frame (typically
     * 16.67ms)
     *
     * @param [in] w The widget to add.
     *
     * @see unregisterTimerWidget
     *
     * @note The framework keeps track of the number of times a specific widget is registered.
     */
    void registerTimerWidget(Drawable* w);

    /** Clears all currently registered timer widgets. */
    void clearAllTimerWidgets();

    /**
     * Removes a widget from the list of widgets receiving ticks every frame (typically
     * 16.67ms) milliseconds.
     *
     * @param [in] w The widget to remove.
     *
     * @note If widget has been registered multiple times, an equal number of calls to unregister
     *       are required to stop widget from receiving tick events.
     */
    void unregisterTimerWidget(const Drawable* w);

    /**
     * gets the number of timer widgets that has been registered.
     *
     * @return The size of timerWidgets.
     */
    uint16_t getNumberOfRegisteredTimerWidgets() const;

    /**
     * Gets the number of timer events registered to a widget, i.e. how many times a
     * drawable must be unregistered until it no longer receives timer ticks.
     *
     * @param  w The widget to to get count from.
     *
     * @return 0 if the drawable is not registered as a timer widget, otherwise returns how
     *         many times the drawable is currently registered.
     */
    uint16_t getTimerWidgetCountForDrawable(const Drawable* w) const;

    static const uint8_t MAX_TIMER_WIDGETS = 32; ///< Maximum number of widgets receiving ticks. @remarks Memory impact: x * (sizeof(Drawable*)+1)
    static const uint16_t TICK_INTERVAL_MS = 10; ///< Deprecated, do not use this constant. Tick interval depends on VSYNC of your target platform.

    /**
     * Sets the DebugPrinter object to be used by the application to print debug messages.
     *
     * @param [in] printer The debug printer to configure.
     */
    static void setDebugPrinter(DebugPrinter* printer)
    {
        debugPrinter = printer;
    }

    /**
     * Returns the DebugPrinter object associated with the application.
     *
     * @return DebugPrinter The DebugPrinter object.
     */
    static DebugPrinter* getDebugPrinter()
    {
        return debugPrinter;
    }

    /**
     * Sets the debug string to be displayed onscreen on top of the framebuffer.
     */
    static void invalidateDebugRegion()
    {
        if (debugPrinter)
        {
            debugRegionInvalidRect.expandToFit(debugPrinter->getRegion());
        }
    }

    /**
     * Sets the debug string to be displayed onscreen on top of the framebuffer.
     *
     * @param [in] string The debug string to display onscreen.
     */
    static void setDebugString(const char* string)
    {
        if (debugPrinter)
        {
            debugPrinter->setString(string);
            invalidateDebugRegion();
        }
    }

protected:
    /**
     * Invalidates this area.
     *
     * @param  area The area to invalidate.
     */
    void invalidateArea(Rect area);

    /** Protected constructor. */
    Application();

    Vector<Drawable*, MAX_TIMER_WIDGETS> timerWidgets; ///< List of widgets that receive timer ticks.
    uint8_t timerWidgetCounter[MAX_TIMER_WIDGETS];     ///< A counter for each potentially registered timer widget. Increase when registering for timer events, decrease when unregistering.
    Vector<Rect, 8> cachedDirtyAreas;                  ///< When draw caching is enabled, these rects keeps track of the dirty screen area.
    Vector<Rect, 8> lastRects;                         ///< The dirty areas from last frame that needs to be redrawn because we have swapped frame buffers.
    Rect redraw;                                       ///< Rect describing application requested invalidate area
    bool drawCacheEnabled;                             ///< True when draw caching is active.
    bool transitionHandled;                            ///< True if the transition is done and Screen::afterTransition has been called.
    static Screen* currentScreen;                      ///< Pointer to currently displayed Screen.
    static Transition* currentTransition;              ///< Pointer to current transition.
    static Application* instance;                      ///< Pointer to the instance of the Application-derived subclass. @note Must be set by subclass constructor!
    static DebugPrinter* debugPrinter;                 ///< Pointer to the DebugPrinter instance.
    static Rect debugRegionInvalidRect;                ///< Invalidated Debug Region
};

} // namespace touchgfx

#endif // APPLICATION_HPP
