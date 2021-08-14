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
 * @file mvp/MVPHeap.hpp
 *
 * Declares the touchgfx::MVPHeap class.
 */
#ifndef MVPHEAP_HPP
#define MVPHEAP_HPP

namespace touchgfx
{
class AbstractPartition;
class MVPApplication;

/**
 * Generic heap class for MVP applications. Serves as a way of obtaining the memory storage
 * areas for presenters, screens, transitions and the concrete application.
 *
 * Subclassed by an application-specific heap which provides the actual storage areas.
 * This generic interface is used only in makeTransition.
 */
class MVPHeap
{
public:
    /**
     * Initializes a new instance of the MVPHeap class.
     *
     * @param [in] pres A memory partition containing enough memory to hold the largest
     *                  presenter.
     * @param [in] scr  A memory partition containing enough memory to hold the largest view.
     * @param [in] tra  A memory partition containing enough memory to hold the largest
     *                  transition.
     * @param [in] app  A reference to the MVPApplication instance.
     */
    MVPHeap(AbstractPartition& pres,
            AbstractPartition& scr,
            AbstractPartition& tra,
            MVPApplication& app)
        : presenterStorage(pres),
          screenStorage(scr),
          transitionStorage(tra),
          frontendApplication(app)
    {
    }

    /** Finalizes an instance of the MVPHeap class. */
    virtual ~MVPHeap()
    {
    }

    AbstractPartition& presenterStorage;  ///< A memory partition containing enough memory to hold the largest presenter.
    AbstractPartition& screenStorage;     ///< A memory partition containing enough memory to hold the largest view.
    AbstractPartition& transitionStorage; ///< A memory partition containing enough memory to hold the largest transition.
    MVPApplication& frontendApplication;  ///< A reference to the MVPApplication instance.
};

} // namespace touchgfx

#endif // MVPHEAP_HPP
