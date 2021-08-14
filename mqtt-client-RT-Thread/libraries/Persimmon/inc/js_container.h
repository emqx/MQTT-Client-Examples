#pragma once

#include <pm_container.h>

class JsContainer : public Persimmon::Container
{
public:
    JsContainer();
    virtual ~JsContainer();

    virtual void setData(void* data);

private:

};

