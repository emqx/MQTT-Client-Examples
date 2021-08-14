#pragma once

#include <pm_dotIndicator.h>

class JsDotIndicator : public Persimmon::DotIndicator
{
public:
    JsDotIndicator();
    virtual ~JsDotIndicator();

    virtual void setData(void* data);

private:

};
