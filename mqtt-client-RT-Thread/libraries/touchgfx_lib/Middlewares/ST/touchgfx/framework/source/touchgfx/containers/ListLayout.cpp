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

#include <touchgfx/containers/ListLayout.hpp>

namespace touchgfx
{
class AdjustElements
{
public:
    AdjustElements(Drawable* d, Direction dir) : insertedCoord(0),
        newElementPassed(false),
        newElement(d),
        direction(dir) {}

    void handleInsert(Drawable& d)
    {
        if (!newElementPassed)
        {
            if (newElement == &d)
            {
                newElementPassed = true;
            }
            else
            {
                if (direction == SOUTH)
                {
                    insertedCoord = d.getY() + d.getHeight();
                }
                else if (direction == EAST)
                {
                    insertedCoord = d.getX() + d.getWidth();
                }
            }
        }
        else
        {
            if (direction == SOUTH)
            {
                d.setY(d.getY() + newElement->getHeight());
            }
            else if (direction == EAST)
            {
                d.setX(d.getX() + newElement->getWidth());
            }
        }
    }

    void handleRemove(Drawable& d)
    {
        if (!newElementPassed)
        {
            if (newElement == &d)
            {
                newElementPassed = true;
            }
        }
        else
        {
            if (direction == SOUTH)
            {
                d.setY(d.getY() - newElement->getHeight());
            }
            else if (direction == EAST)
            {
                d.setX(d.getX() - newElement->getWidth());
            }
        }
        if (newElement != &d)
        {
            if (direction == SOUTH)
            {
                if (d.getWidth() > insertedCoord)
                {
                    insertedCoord = d.getWidth();
                }
            }
            else if (direction == EAST)
            {
                if (d.getHeight() > insertedCoord)
                {
                    insertedCoord = d.getHeight();
                }
            }
        }
    }

    int16_t insertedCoord;
    bool newElementPassed;

private:
    Drawable* newElement;
    Direction direction;
}; //lint !e1712

void ListLayout::internalAddElementAt(Drawable& d, int16_t coord)
{
    switch (direction)
    {
    case SOUTH:
        if (rect.width < d.getWidth())
        {
            rect.width = d.getWidth();
        }
        rect.height += d.getHeight();
        d.setXY(0, coord);
        offset += d.getHeight();
        break;
    case EAST:
        if (rect.height < d.getHeight())
        {
            rect.height = d.getHeight();
        }
        rect.width += d.getWidth();
        d.setXY(coord, 0);
        offset += d.getWidth();
        break;
    case NORTH:
    case WEST:
    default:
        break;
    }
}

void ListLayout::internalAddElement(Drawable& d)
{
    internalAddElementAt(d, offset);
}

void ListLayout::internalRemoveElement(Drawable& d, int16_t coord)
{
    switch (direction)
    {
    case SOUTH:
        if (rect.width > coord)
        {
            rect.width = coord;
        }
        rect.height -= d.getHeight();
        d.setX(0);
        d.setY(0);
        offset -= d.getHeight();
        break;
    case EAST:
        if (rect.height > coord)
        {
            rect.height = coord;
        }
        rect.width -= d.getWidth();
        d.setX(0);
        d.setY(0);
        offset -= d.getWidth();
        break;
    case NORTH:
    case WEST:
    default:
        break;
    }
}

void ListLayout::setDirection(const Direction d)
{
    assert((d == SOUTH || d == EAST) && "Chosen direction not supported");
    if (direction != d)
    {
        direction = d;
        offset = 0;
        setWidth(0);
        setHeight(0);
        Callback<ListLayout, Drawable&> function(this, &ListLayout::internalAddElement);
        forEachChild(&function);
        if (parent)
        {
            parent->childGeometryChanged();
        }
    }
}

void ListLayout::add(Drawable& d)
{
    internalAddElement(d);
    Container::add(d);
    if (parent)
    {
        parent->childGeometryChanged();
    }
}

void ListLayout::removeAll()
{
    offset = 0;
    setWidth(0);
    setHeight(0);
    Container::removeAll();
    if (parent)
    {
        parent->childGeometryChanged();
    }
}

void ListLayout::insert(Drawable* previous, Drawable& d)
{
    if (!firstChild)
    {
        // List is empty, just add the new entry
        add(d);
        return;
    }
    Container::insert(previous, d);
    AdjustElements tmp(&d, direction);
    Callback<AdjustElements, Drawable&> function(&tmp, &AdjustElements::handleInsert);
    forEachChild(&function);
    internalAddElementAt(d, tmp.insertedCoord);
    if (parent)
    {
        parent->childGeometryChanged();
    }
}

void ListLayout::remove(Drawable& d)
{
    AdjustElements tmp(&d, direction);
    Callback<AdjustElements, Drawable&> function(&tmp, &AdjustElements::handleRemove);
    forEachChild(&function);
    if (tmp.newElementPassed)
    {
        internalRemoveElement(d, tmp.insertedCoord);
        Container::remove(d);
    }
    if (parent)
    {
        parent->childGeometryChanged();
    }
}
} // namespace touchgfx
