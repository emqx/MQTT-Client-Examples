#pragma once

#include "js_object.h"
#include <pm_checkbox.h>


class JsCheckbox : public Persimmon::Checkbox
{
public:
    JsCheckbox();
    virtual ~JsCheckbox();

    void setJsFunction(jerry_value_t object, const char* func);

    void onSignal(int id, bool s);

    virtual void setData(void* data);

private:
    jerry_value_t jsCallFunc;
    jerry_value_t jsObj;
};

