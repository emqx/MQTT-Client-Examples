#pragma once

#include "js_object.h"
#include <pm_canvas.h>

class JsCanvas : public Persimmon::Canvas
{
public:
    JsCanvas();

    virtual void setData(void* data);

protected:
};
