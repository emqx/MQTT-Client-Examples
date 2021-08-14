#pragma once

#include <pm_label.h>
#include <js_object.h>

using namespace Persimmon;

class JsTextBox : public JsObject, public Persimmon::Label
{
public:
    JsTextBox();
    virtual ~JsTextBox();

    void setKeyboard(const char *url);
    void setText(const char *text);

    virtual bool handleGestureEvent(struct rtgui_event_gesture *, const struct rtgui_gesture *);
    virtual void setData(void* data);
    virtual void getData(void *ret, const std::string &name) const;

    void setJsFuncChanged(jerry_value_t object, const char* func);
    void onChanged(jerry_value_t text);

protected:
    jerry_value_t jsCallFuncChanged;
    jerry_value_t jsObj;
};
