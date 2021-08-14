#pragma once

#include <animated_image.h>
#include "jerryscript.h"

class JsAnimatedImage : public Persimmon::AnimatedImage
{
public:
    JsAnimatedImage();
    JsAnimatedImage(const JsAnimatedImage &other);
    virtual ~JsAnimatedImage();

    virtual Widget *depthCopy(void) const;
    void setJsFunction(jerry_value_t object, const char *func);
    void onSignal(int id);
    virtual void setData(void *data);

private:
    jerry_value_t jsObj, jsCallFunc;
};
