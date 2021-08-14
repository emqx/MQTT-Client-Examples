#pragma once

#include "js_object.h"
#include <pm_wheel.h>

class JsWheelString : public Persimmon::WheelString
{
public:
    JsWheelString();
    virtual ~JsWheelString();

    void setJsFuncChanged(jerry_value_t object, const char* func);
    void setJsFuncTap(jerry_value_t object, const char* func);

    void onChanged(unsigned int);
    void onTap(unsigned int);
    void onChangedS(std::string);
    void onTapS(std::string);

    virtual void setData(void* data);

private:
    jerry_value_t jsCallFuncChanged, jsCallFuncTap;
    jerry_value_t jsObj;
};
