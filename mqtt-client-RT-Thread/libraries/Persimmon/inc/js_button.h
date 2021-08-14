#pragma once

#include "js_object.h"
#include <pm_button.h>
#include <pm_image.h>

class JsButton : public Persimmon::Button
{
public:
    JsButton();
    //JsButton(const JsButton &other);
    virtual ~JsButton();

    //virtual Widget *depthCopy(void) const;
    void setJsFunction(jerry_value_t object, const char *func);
    void onSignal(int type);
    virtual void setData(void *data);

private:
    jerry_value_t jsObj, jsCallFunc, jsCustomProperty;
};

