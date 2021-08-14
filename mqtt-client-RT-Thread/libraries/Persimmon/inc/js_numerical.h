#pragma once

#include <pm_numerical.h>

class JsNumerical : public Persimmon::Numerical
{
public:
    JsNumerical();
    virtual ~JsNumerical();

    virtual void setData(void* data);

private:

};

