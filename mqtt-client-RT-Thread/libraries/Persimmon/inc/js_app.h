#pragma once

#include "js_object.h"
#include "js_window.h"
#include "js_page.h"

#include <pm_application.h>

#include <vector>
#include <string>
#include <list>

#define GUI_DISP_WATERMARK

namespace Persimmon
{
class Label;
class ImageBox;
};

class JsApp : public JsObject, public Persimmon::Application
{
public:
    JsApp(jerry_value_t obj);
    virtual ~JsApp();

    virtual void initJsObject(void);
    void onLaunch(void);
    void onExit(void);
    void onUpdate(jerry_value_t obj);
    void onShow(void);
    void jsWindowShow(void);

    static JsApp *getAppSelf();
    virtual rt_bool_t eventHandler(struct rtgui_event *event);

    bool loadPage(const char* url);
    void navigate(jerry_value_t value, jerry_value_t num = 0);
    bool navigateTo(jerry_value_t obj);
    bool redirectTo(jerry_value_t obj);
    void navigateBack(jerry_value_t obj, int depth = 1);
    bool closePage(jerry_value_t name);
    double getCurrentPages(void)
    {
        return (double)window()->getPageSize();
    }

    JsPage* getLastPage(unsigned int index = 0)
    {
        return (JsPage*)window()->getLastPage(index);
    }

    JsPage* getCurrentPage(void)
    {
        return (JsPage*)window()->getCurrentPage();
    }

    JsPage* getPagebyName(const char *name)
    {
        return (JsPage*)window()->getPagebyName(name);
    }

    std::string &getJumpPageXml()
    {
        return jumpPageXml;
    }

    JsWindow* getWindow(void)
    {
        return (JsWindow*)window();
    }

    void setJumpPage(JsPage* page)
    {
        jumpPage = page;
    }

    std::string getAppPath()
    {
        return appPath;
    }

    void updateFpsLabel(void);
    void addListener(jerry_value_t name, jerry_value_t func);

private:
    bool callListener(const char *name, jerry_value_t value);
    void clearListener();

    std::string appPath, jumpPageXml;
    JsPage *jumpPage;
#ifdef GUI_DISP_WATERMARK
    ImageBox* m_watermark;
#endif
    Listener listener;
};

