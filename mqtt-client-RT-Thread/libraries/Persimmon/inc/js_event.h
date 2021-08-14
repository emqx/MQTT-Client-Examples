#pragma once

#include <rtthread.h>
#include <js_object.h>

class JsEvent : public JsObject
{
public:
    JsEvent();
    virtual ~JsEvent();

    void setType(const char *type);
    void setTimeStamp(void);

    void setTarget(const char *id, const char *tagName, JsObject *dataset = NULL);
    void setCurrentTarget(const char *id, const char *tagName, JsObject *dataset = NULL);

    void setDetail(JsObject *detail);
    void setTouchs(JsObject *touchs, int size);

private:
};

