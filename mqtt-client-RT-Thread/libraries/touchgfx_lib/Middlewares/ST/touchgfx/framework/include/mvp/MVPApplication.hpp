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
 * @file mvp/MVPApplication.hpp
 *
 * Declares the touchgfx::MVPApplication class.
 */
#ifndef MVPAPPLICATION_HPP
#define MVPAPPLICATION_HPP

#include <cassert>
#include <new>
#include <common/Meta.hpp>
#include <common/Partition.hpp>
#include <mvp/MVPHeap.hpp>
#include <mvp/View.hpp>
#include <touchgfx/Application.hpp>
#include <touchgfx/Callback.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/transitions/Transition.hpp>

namespace touchgfx
{
class Presenter;

/**
 * A specialization of the TouchGFX Application class that provides the necessary glue for
 * transitioning between presenter/view pairs.
 *
 * It maintains a callback for transitioning and evaluates this at each tick.
 *
 * @see Application
 */
class MVPApplication : public Application
{
public:
    /** Initializes a new instance of the MVPApplication class. */
    MVPApplication()
        : currentPresenter(0),
          pendingScreenTransitionCallback(0)
    {
        instance = this;
    }

    /**
     * Handles the pending screen transition.
     *
     * Delegates the work to evaluatePendingScreenTransition()
     */
    virtual void handlePendingScreenTransition()
    {
        evaluatePendingScreenTransition();
    }

protected:
    Presenter* currentPresenter; ///< Pointer to the currently active presenter.

    GenericCallback<>* pendingScreenTransitionCallback; ///< Callback for screen transitions. Will be set to something valid when a transition request is made.

    /**
     * Evaluates the pending Callback instances. If a callback is valid, it is executed and
     * a Screen transition is executed.
     */
    void evaluatePendingScreenTransition()
    {
        if (pendingScreenTransitionCallback && pendingScreenTransitionCallback->isValid())
        {
            pendingScreenTransitionCallback->execute();
            pendingScreenTransitionCallback = 0;
        }
    }
};

/**
 * Prepare screen transition. Private helper function for makeTransition. Do not use.
 *
 * @param [in] currentScreen    If non-null, the current screen.
 * @param [in] currentPresenter If non-null, the current presenter.
 * @param [in] currentTrans     If non-null, the current transaction.
 */
FORCE_INLINE_FUNCTION static void prepareTransition(Screen** currentScreen, Presenter** currentPresenter, Transition** currentTrans)
{
    Application::getInstance()->clearAllTimerWidgets();

    if (*currentTrans)
    {
        (*currentTrans)->tearDown();
    }
    if (*currentTrans)
    {
        (*currentTrans)->~Transition();
    }
    if (*currentScreen)
    {
        (*currentScreen)->tearDownScreen();
    }
    if (*currentPresenter)
    {
        (*currentPresenter)->deactivate();
    }
    if (*currentScreen)
    {
        (*currentScreen)->~Screen();
    }
    if (*currentPresenter)
    {
        (*currentPresenter)->~Presenter();
    }
}

/**
 * Finalize screen transition. Private helper function for makeTransition. Do not use.
 *
 * @param [in] newScreen     If non-null, the new screen.
 * @param [in] newPresenter  If non-null, the new presenter.
 * @param [in] newTransition If non-null, the new transition.
 */
FORCE_INLINE_FUNCTION static void finalizeTransition(Screen* newScreen, Presenter* newPresenter, Transition* newTransition)
{
    newScreen->setupScreen();
    newPresenter->activate();
    newScreen->bindTransition(*newTransition);
    newTransition->init();
    newTransition->invalidate();
}

/**
 * Function for effectuating a screen transition (i.e. makes the requested new presenter/view
 * pair active). Once this function has returned, the new screen has been transitioned
 * to. Due to the memory allocation strategy of using the same memory area for all
 * screens, the old view/presenter will no longer exist when this function returns.
 *
 * Will properly clean up old screen (tearDownScreen, Presenter::deactivate) and call
 * setupScreen/activate on new view/presenter pair. Will also make sure the view,
 * presenter and model are correctly bound to each other.
 *
 * @tparam ScreenType    Class type for the View.
 * @tparam PresenterType Class type for the Presenter.
 * @tparam TransType     Class type for the Transition.
 * @tparam ModelType     Class type for the Model.
 * @param [in] currentScreen    Pointer to pointer to the current view.
 * @param [in] currentPresenter Pointer to pointer to the current presenter.
 * @param [in] heap             Reference to the heap containing the memory storage in which
 *                              to allocate.
 * @param [in] currentTrans     Pointer to pointer to the current transition.
 * @param [in] model            Pointer to model.
 *
 * @return Pointer to the new Presenter of the requested type. Incidentally it will be the same
 *         value as the old presenter due to memory reuse.
 */
template <class ScreenType, class PresenterType, class TransType, class ModelType>
PresenterType* makeTransition(Screen** currentScreen, Presenter** currentPresenter, MVPHeap& heap, Transition** currentTrans, ModelType* model)
{
    assert(sizeof(ScreenType) <= heap.screenStorage.element_size() && "View allocation error: Check that all views are added to FrontendHeap::ViewTypes");
    assert(sizeof(PresenterType) <= heap.presenterStorage.element_size() && "Presenter allocation error: Check that all presenters are added to FrontendHeap::PresenterTypes");
    assert(sizeof(TransType) <= heap.transitionStorage.element_size() && "Transition allocation error: Check that all transitions are added to FrontendHeap::TransitionTypes");

    prepareTransition(currentScreen, currentPresenter, currentTrans);

    TransType* newTransition = new (&heap.transitionStorage.at<TransType>(0)) TransType;
    ScreenType* newScreen = new (&heap.screenStorage.at<ScreenType>(0)) ScreenType;
    PresenterType* newPresenter = new (&heap.presenterStorage.at<PresenterType>(0)) PresenterType(*newScreen);
    *currentTrans = newTransition;
    *currentPresenter = newPresenter;
    *currentScreen = newScreen;
    model->bind(newPresenter);
    newPresenter->bind(model);
    newScreen->bind(*newPresenter);

    finalizeTransition((Screen*)newScreen, (Presenter*)newPresenter, (Transition*)newTransition);

    return newPresenter;
}

} // namespace touchgfx

#endif // MVPAPPLICATION_HPP
