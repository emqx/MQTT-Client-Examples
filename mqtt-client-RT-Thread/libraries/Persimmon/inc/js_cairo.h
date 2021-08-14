#pragma once

#include "js_object.h"

#ifdef PKG_USING_CAIRO

#include <pm_cairo.h>

class JsCairo : public Persimmon::Cairo
{
public:
    JsCairo();
    virtual ~JsCairo();

    virtual void setData(void* data);

    void setJsFunction(jerry_value_t object, const char* func);
    void bindTouch(const char *type, const struct rtgui_gesture *gest);
    virtual bool handleGestureEvent(struct rtgui_event_gesture *gev, const struct rtgui_gesture *gest);

protected:
    jerry_value_t jsCallFunc;
    jerry_value_t jsObj;
    struct rtgui_gesture lastGest;
};

#endif
