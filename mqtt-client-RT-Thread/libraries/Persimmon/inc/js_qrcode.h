#pragma once

#include "js_object.h"
#include <pm_qrcode.h>

class JsQRCode : public Persimmon::QRCode
{
public:
    JsQRCode();

    virtual void setData(void* data);

protected:
};
