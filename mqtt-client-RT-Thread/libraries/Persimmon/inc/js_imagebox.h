#pragma once

#include <image_box.h>

class JsImageBox : public Persimmon::ImageBox
{
public:
    JsImageBox();
    virtual ~JsImageBox();

    virtual void setData(void* data);

protected:

private:

};

