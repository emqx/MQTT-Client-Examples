#ifndef __JS_ARCBAR_H
#define __JS_ARCBAR_H

#include "js_object.h"
#include <pm_arcbar.h>
#ifdef PKG_USING_VGL

class JsArcBar : public Persimmon::ArcBar
{
public:
    JsArcBar();
    virtual ~JsArcBar();

    virtual void setData(void* data);
};

class JsArcDotIndicator : public Persimmon::ArcDotIndicator
{
public:
    JsArcDotIndicator();
    virtual ~JsArcDotIndicator();

    virtual void setData(void* data);
};

#endif
#endif
