#pragma once

#include "js_object.h"
#include <pm_wheel.h>

class JsCarouselImage : public Persimmon::CarouselImage
{
public:
    JsCarouselImage();
    virtual ~JsCarouselImage();

    void setJsFuncChanged(jerry_value_t object, const char* func);
    void setJsFuncTap(jerry_value_t object, const char* func);

    void onChanged(int value);
    void onTap(int value);

    virtual void setData(void* data);

private:
    jerry_value_t jsCallFuncChanged, jsCallFuncTap;
    jerry_value_t jsObj;
};
