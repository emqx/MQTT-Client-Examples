#pragma once

#include <pm_clock.h>
#include <resource.h>
#include <jerry_util.h>

class JsClock : public Persimmon::Clock
{
public:
    JsClock();
    virtual ~JsClock();

    virtual void setData(void* data);

private:
    struct PointerInfo
    {
        Image *image;
        PointF position;
        bool validPos;
    };
    PointerInfo parsePointer(jerry_value_t value);

private:

};

