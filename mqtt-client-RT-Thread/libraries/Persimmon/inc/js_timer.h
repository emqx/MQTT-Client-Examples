#pragma once

#include "js_object.h"

#include <pm_timer.h>

class JsTimer : public Persimmon::Timer
{
public:
    JsTimer(jerry_value_t obj, int tick, Timer::flag flag);
    ~JsTimer();

    void onTimeout();

private:
    jerry_value_t timeoutObj;
};

class JsSyncTimer : public Persimmon::SyncTimer
{
public:
    JsSyncTimer(jerry_value_t function, jerry_value_t name = 0);
    ~JsSyncTimer();

    void onTimeout();

private:
    jerry_value_t m_timeoutFunction, timerName;
};
