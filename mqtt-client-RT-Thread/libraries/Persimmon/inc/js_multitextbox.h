#pragma once

#include <pm_multi_textbox.h>
#include <jerryscript.h>

class JsMultiTextBox : public Persimmon::MultiTextBox
{
public:
    JsMultiTextBox();
    JsMultiTextBox(const JsMultiTextBox &other);
    virtual ~JsMultiTextBox();

    virtual Widget *depthCopy(void) const;
    void onSignal(int h);
    virtual void setData(void *data);

private:
    jerry_value_t jsObj, jsCallFunc;
};
