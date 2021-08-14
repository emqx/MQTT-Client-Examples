/*
* File      : main_win.h
* COPYRIGHT (C) 2012-2017, Shanghai Real-Thread Technology Co., Ltd
*
* Change Logs:
* Date           Author       Notes
* 2018-09-01     XY           the first version
*/

#pragma once

#include <pm_window.h>
#include <pm_timer.h>
#include <pm_label.h>
#include <pm_wheel.h>
#include <pm_button.h>

using namespace Persimmon;

class MainWin : public Window
{
public:
    enum {
        LABEL,
        BUTTON,
        CHECKBOX,
        SWITCH,
        CARD,
        AINMIMAGE,
        CIRCLEPROGRESS,
        CONTAINERBUTTON,
        IMAGEBOX,
        CAIRO,
        CLOCK,
        DOT,
        LISTCTRL,
        MULTITEXTBOX,
        NUMERICAL,
        PROGRESSBAR,
        SLIDER,
        QRCODE,
        WHEEL,
    };

    MainWin(const char *title);

    void onButton(int num);
    void onTap(int num);
    virtual void pageEmpty(void);

private:
    Label *label;
    WheelString *strWheel;
    Button *button0, *button1;
};

extern "C" {
    void rtgui_main_app(void);
}
