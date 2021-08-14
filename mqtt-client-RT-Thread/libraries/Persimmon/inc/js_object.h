#pragma once

#include <rtthread.h>
#include <jerryscript.h>
//#include <ecma-globals.h>
#include <jerry_util.h>

class JsObject
{
public:
    JsObject();
    JsObject(jerry_value_t obj);
    virtual ~JsObject();

    jerry_value_t getJsObject(void)
    {
        return js_object;
    }
    void setJsObject(jerry_value_t obj);

    jerry_value_t getProperty(const char* name);

    void setProperty(const char* name, const jerry_value_t value);
    void setProperty(const char* name, const char* value);
    void setProperty(const char* name, bool value);
    void setProperty(const char* name, double value);
    void setProperty(const char* name, JsObject &obj);
    void setPrototype(const jerry_value_t value);

    bool callFunction(const char* name, const jerry_value_t args_p[], jerry_size_t args_count);
    void addFunction(const char* name, void *func);

    virtual void initJsObject(void);

    /* dump js value information */
    void dump(void);

protected:
    jerry_value_t js_object;
};

