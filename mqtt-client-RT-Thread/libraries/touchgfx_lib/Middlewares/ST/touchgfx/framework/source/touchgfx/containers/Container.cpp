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

#include <touchgfx/containers/Container.hpp>
#include <touchgfx/Screen.hpp>

namespace touchgfx
{
bool Container::contains(const Drawable& d)
{
    bool found = false;
    Drawable* head = firstChild;
    while (head && !found)
    {
        found = (head == &d);
        head = head->nextSibling;
    }
    return found;
}

void Container::add(Drawable& d)
{
    assert(&d != this && "Cannot add Drawable to self");
    assert(d.parent == 0 && "Cannot add Drawable multiple times");

    // Initialize d to have this as parent and no sibling.
    d.parent = this;
    d.nextSibling = 0;

    // Check if d is the first child to be added (container is empty)
    if (!firstChild)
    {
        firstChild = &d;
    }
    else
    {
        Drawable* head = firstChild;
        // Skip to end of currently added children
        while (head->nextSibling)
        {
            assert(head != &d && "Cannot add Drawable multiple times");
            head = head->nextSibling;
        }
        assert(head != &d && "Cannot add Drawable multiple times");
        // Make last child now point to d.
        head->nextSibling = &d;
    }
}

void Container::remove(Drawable& d)
{
    if (!firstChild)
    {
        // No children
        return;
    }

    if (&d == firstChild)
    {
        // d is first child.
        d.parent = 0;
        if (!d.nextSibling)
        {
            // d was only child, so now this container is empty
            firstChild = 0;
        }
        else
        {
            firstChild = d.nextSibling;
            d.nextSibling = 0;
        }
        return;
    }
    Drawable* tmp = firstChild;

    while (tmp)
    {
        if (tmp->nextSibling == &d)
        {
            tmp->nextSibling = d.nextSibling;
            d.parent = 0;
            d.nextSibling = 0;
            return;
        }
        else
        {
            tmp = tmp->nextSibling;
        }
    }
}

void Container::removeAll()
{
    while (firstChild)
    {
        Drawable* d = firstChild;
        firstChild = firstChild->nextSibling;
        d->parent = 0;
        d->nextSibling = 0;
    }
}

void Container::unlink()
{
    firstChild = 0;
}

void Container::draw(const Rect& invalidatedArea) const
{
    // The draw function of Container is not normally used. Containers do not per default
    // appear in the draw chain, since they are normally invisible themselves. However,
    // if someone decides to call draw on a container, at least do something useful (draw children).
    if (!isVisible() || !firstChild)
    {
        // Nothing to draw
        return;
    }

    Rect tmp = invalidatedArea;
    Drawable* d = firstChild;
    while (d)
    {
        if (d->isVisible())
        {
            Rect drawableRegion = tmp & d->getRect();
            if (!drawableRegion.isEmpty())
            {
                // This child has a non-empty intersection with the invalidated area.
                // Convert region to the Drawable's coordinate system and draw.
                drawableRegion.x -= d->getX();
                drawableRegion.y -= d->getY();
                d->draw(drawableRegion);
            }
        }
        d = d->nextSibling;
    }
}

void Container::getLastChild(int16_t x, int16_t y, Drawable** last)
{
    // This function is used to obtain the drawable that should receive a click/drag/gesture event.
    // Find the last child (ie. the last child that was added, ie. the "front-most" drawable) covering
    // the specified coords.
    if (isTouchable())
    {
        // If the container itself is touchable, result so far is "this". Might be overridden by a child.
        *last = this;
    }

    Drawable* d = firstChild;
    while (d)
    {
        // Iterate over children.
        if (d->isVisible() && d->getRect().intersect(x, y))
        {
            int16_t xadj = x - d->getX();
            int16_t yadj = y - d->getY();
            d->getLastChild(xadj, yadj, last);
        }
        d = d->nextSibling;
    }
}

Rect Container::getSolidRect() const
{
    return Rect(0, 0, 0, 0);
}

Rect Container::getContainedArea() const
{
    Drawable* d = firstChild;
    Rect contained(0, 0, 0, 0);
    while (d)
    {
        contained.expandToFit(d->getRect());
        d = d->nextSibling;
    }
    return contained;
}

void Container::moveChildrenRelative(int16_t deltaX, int16_t deltaY)
{
    Drawable* d = firstChild;
    while (d)
    {
        d->moveRelative(deltaX, deltaY);
        d = d->nextSibling;
    }
}

void Container::forEachChild(GenericCallback<Drawable&>* function)
{
    Drawable* d = firstChild;
    while (d)
    {
        function->execute(*d);
        d = d->nextSibling;
    }
}

void Container::insert(Drawable* previous, Drawable& d)
{
    if (!firstChild)
    {
        // Insert as only element
        add(d);
        return;
    }
    else if (!previous)
    {
        // Insert as head element
        d.nextSibling = firstChild;
        firstChild = &d;
        d.parent = this;
    }
    else
    {
        Drawable* tmp = firstChild;
        while (tmp)
        {
            if (tmp == previous)
            {
                d.nextSibling = tmp->nextSibling;
                tmp->nextSibling = &d;
                d.parent = this;
                return;
            }
            tmp = tmp->nextSibling;
        }
    }
}

void Container::setupDrawChain(const Rect& invalidatedArea, Drawable** nextPreviousElement)
{
    // This function adds the children of this container to the list of drawables to draw.
    if (!isVisible())
    {
        // If this container itself is not visible, do not add anyone to draw chain.
        return;
    }

    if (!firstChild)
    {
        // If this container is empty, do not add anyone.
        return;
    }
    Drawable* d = firstChild;
    while (d)
    {
        if (d->isVisible())
        {
            // Only drawables intersecting with the specified invalidated area will be added.
            Rect drawableRegion = invalidatedArea & d->getRect();
            if (!drawableRegion.isEmpty())
            {
                drawableRegion.x -= d->getX();
                drawableRegion.y -= d->getY();
                d->setupDrawChain(drawableRegion, nextPreviousElement);
            }
        }
        d = d->nextSibling;
    }
}
} // namespace touchgfx
