/*
 * File      : pm_checkbox.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */

#pragma once

#include <vector>
#include <pm_container.h>
#include <pm_image.h>
#include "sigslot.h"

namespace Persimmon
{

class Checkbox : public Widget
{
public:
    Checkbox();
    Checkbox(const Rect& rect);
    virtual ~Checkbox();

    bool isSelected(void)
    {
        return selected;
    }

    void enableSelected(bool sel = true)
    {
        selected = sel;
        invalidate();
    }

    void setAutoRefresh(bool au)
    {
        autoRefresh = au;
    }

    void setSelectedImg(Image *image)
    {
        if (selectedImg)
            delete selectedImg;

        selectedImg = NULL;

        if (image)
            selectedImg = image;
    }

    Image *getSelectedImg(void)
    {
        return selectedImg;
    }

    void setNormalImg(Image *image)
    {
        if (normalImg)
            delete normalImg;

        normalImg = NULL;

        if (image)
            normalImg = image;
    }

    Image *getNormalImg(void)
    {
        return normalImg;
    }

    Signal<int, bool> changed;

    virtual bool handleGestureEvent(struct rtgui_event_gesture *, const struct rtgui_gesture *);

    virtual void render(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

private:
    Image *selectedImg, *normalImg;
    bool selected, autoRefresh;
};

class GroupBox : public Container
{
public:
    GroupBox();
    GroupBox(const Rect& rect);
    virtual ~GroupBox();

    virtual void handleSignal(int num, bool sel)
    {
        if (sel == true)
        {
            if (selCheckboxNum != num)
            {
                this->checkbox[selCheckboxNum]->enableSelected(false);
                this->checkbox[selCheckboxNum]->invalidate();
                selCheckboxNum = num;
                changed(getNumber(), selCheckboxNum);
            }
        }
        else
        {
            this->checkbox[selCheckboxNum]->enableSelected(true);
            this->checkbox[selCheckboxNum]->invalidate();
        }
    }

    void addCheckbox(Checkbox* widget)
    {
        if (widget != NULL)
        {
            this->checkbox.push_back(widget);
            widget->setNumber(this->checkbox.size() - 1);
            widget->changed.connect(this, &GroupBox::handleSignal);
            this->addChild(widget);
        }
    }

    void selectCheckbox(int num = 0)
    {
        if (selCheckboxNum != num)
            this->checkbox[selCheckboxNum]->enableSelected(false);

        selCheckboxNum = num;
        this->checkbox[selCheckboxNum]->enableSelected();
    }

    int getSelectNum(void)
    {
        return selCheckboxNum;
    }

    Signal<int, int> changed;

private:
    std::vector<Checkbox*> checkbox;
    int selCheckboxNum;
};

}
