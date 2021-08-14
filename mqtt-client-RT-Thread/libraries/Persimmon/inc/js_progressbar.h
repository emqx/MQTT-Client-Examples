#pragma once

#include <pm_progressbar.h>

class JsProgressBar : public Persimmon::ProgressBar
{
public:
    JsProgressBar();
    virtual ~JsProgressBar();

    virtual void setData(void* data);

private:

};

