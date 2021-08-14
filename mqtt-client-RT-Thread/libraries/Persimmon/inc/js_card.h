#pragma once

#include "js_object.h"
#include <card.h>

class JsCard : public Persimmon::Card
{
public:
    JsCard();
    /*JsCard(const JsCard &other);*/
    virtual ~JsCard();

    /*virtual Widget *depthCopy(void) const;*/
    void setJsFunction(jerry_value_t object, const char *func);
    void onSignal(int id, int value);
    virtual void setData(void *data);

protected:
    virtual void moveEvent(CardEvent event, int index);

private:
    jerry_value_t jsObj, jsCallFunc;
};
