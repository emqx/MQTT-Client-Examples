#pragma once

#include "js_object.h"
#include <pm_barcode.h>

class JsBarCode : public Persimmon::BarCode
{
public:
    JsBarCode();

    virtual void setData(void* data);

protected:
};
