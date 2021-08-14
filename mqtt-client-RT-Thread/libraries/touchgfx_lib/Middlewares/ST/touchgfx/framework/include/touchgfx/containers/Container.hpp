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
 * @file touchgfx/containers/Container.hpp
 *
 * Declares the touchgfx::Container class.
 */
#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include <touchgfx/Callback.hpp>
#include <touchgfx/Drawable.hpp>

namespace touchgfx
{
/**
 * A Container is a Drawable that can have child nodes. The z-order of children is determined by
 * the order in which Drawables are added to the container - the Drawable added last
 * will be front-most on the screen.
 *
 * This class overrides a few functions in Drawable in order to traverse child nodes.
 *
 * Note that containers act as view ports - that is, only the parts of children that
 * intersect with the geometry of the container will be visible (e.g. setting a
 * container's width to 0 will render all children invisible).
 *
 * @see Drawable
 */
class Container : public Drawable
{
public:
    Container()
        : Drawable(),
          firstChild(0)
    {
    }

    /**
     * Adds a Drawable instance as child to this Container. The Drawable added will be
     * placed as the element to be drawn last, and thus appear on top of all previously
     * added drawables in the Container.
     *
     * @param [in] d The Drawable to add.
     *
     * @note Never add a drawable more than once!
     */
    virtual void add(Drawable& d);

    /**
     * Removes a Drawable from the container by removing it from the linked list of
     * children. If the Drawable is not in the list of children, nothing happens. It is
     * possible to remove an element from whichever Container it is a member of using:
     * @code
     *      if (d.getParent()) d.getParent()->remove(d);
     * @endcode
     * The Drawable will have the parent and next sibling cleared, but is otherwise left
     * unaltered.
     *
     * @param [in] d The Drawable to remove.
     *
     * @note This is safe to call even if d is not a child of this Container (in which case nothing happens).
     */
    virtual void remove(Drawable& d);

    /**
     * Removes all children in the Container by resetting their parent and sibling pointers.
     * Please note that this is not done recursively, so any child which is itself a
     * Container is not emptied.
     */
    virtual void removeAll();

    /**
     * Removes all children by unlinking the first child. The parent and sibling pointers of
     * the children are not reset.
     *
     * @see getFirstChild
     */
    virtual void unlink();

    /**
     * Query if a given Drawable has been added directly to this Container. The search is
     * not done recursively.
     *
     * @param  d The Drawable to look for.
     *
     * @return True if the specified Drawable instance is direct child of this container,
     *         false otherwise.
     */
    virtual bool contains(const Drawable& d);

    /**
     * Inserts a Drawable after a specific child node. If previous child node is 0, the
     * drawable will be inserted as the first element in the list. The first element in the
     * list of children is the element drawn first, so this makes it possible to insert a
     * Drawable \a behind all previously added children.
     *
     * @param [in] previous The Drawable to insert after. If null, insert as header.
     * @param [in] d        The Drawable to insert.
     *
     * @note As with add, do not add the same drawable twice.
     */
    virtual void insert(Drawable* previous, Drawable& d);

    /**
     * Gets the last child in the list of children in this Container. If this Container is
     * touchable (isTouchable()), it will be passed back as the result. Otherwise all \a
     * visible children are traversed recursively to find the Drawable that intersects with
     * the given coordinate.
     *
     * @param       x    The x coordinate of the intersection.
     * @param       y    The y coordinate of the intersection.
     * @param [out] last out parameter in which the result is placed.
     *
     * @see isVisible, isTouchable
     */
    virtual void getLastChild(int16_t x, int16_t y, Drawable** last);

    virtual void draw(const Rect& invalidatedArea) const;

    virtual Rect getSolidRect() const;

    /**
     * Executes the specified callback function for each child in the Container. The
     * callback to execute must have the following prototype: void T::func(Drawable&amp;)
     *
     * @param [in] function The function to be executed for each child.
     */
    virtual void forEachChild(GenericCallback<Drawable&>* function);

    /**
     * Obtain a pointer to the first child of this container. The first child is the
     * Drawable drawn first, and therefore the Drawable \a behind all other children of this
     * Container. Useful if you want to manually iterate the children added to this
     * container.
     *
     * @return Pointer to the first drawable added to this container. If nothing has been
     *         added return zero.
     *
     * @see getNextSibling
     */
    Drawable* getFirstChild()
    {
        return firstChild;
    }

protected:
    /**
     * Gets a rectangle describing the total area covered by the children of this container.
     *
     * @return Rectangle covering all children.
     */
    virtual Rect getContainedArea() const;

    /**
     * Calls moveRelative on all children.
     *
     * @param  deltaX Horizontal displacement.
     * @param  deltaY Vertical displacement.
     */
    virtual void moveChildrenRelative(int16_t deltaX, int16_t deltaY);

    Drawable* firstChild; ///< Pointer to the first child of this container. Subsequent children can be found through firstChild's nextSibling.

    friend class Screen;
    /// @cond
    virtual void setupDrawChain(const Rect& invalidatedArea, Drawable** nextPreviousElement);
    /// @endcond
};

} // namespace touchgfx

#endif // CONTAINER_HPP
