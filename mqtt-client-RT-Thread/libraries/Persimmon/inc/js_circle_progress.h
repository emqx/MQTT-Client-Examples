#pragma once

#include <circle_progress.h>

class JsCircleProgress : public Persimmon::CircleProgress
{
public:
    JsCircleProgress();
    virtual ~JsCircleProgress();

    virtual void setData(void* data);

private:

};

