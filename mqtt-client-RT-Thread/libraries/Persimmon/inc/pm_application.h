/*
 * File      : pm_application.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */

#pragma once

#include <rtgui/rtgui_app.h>
#include <rtgui/rtgui_system.h>
// for utils
#include <pm_widget.h>
#include <vector>
#include <list>

#define App()       (Application::getApplicationSelf())

DEFINE_CLASS_ENUM_FLAG_OPERATORS2(rtgui_app_flag);

namespace Persimmon
{

class Window;
class SyncTimer;

class Application : private utils::noncopyable<Application>
{
public:
    Application(const char *title);
    virtual ~Application();

    virtual rt_bool_t eventHandler(struct rtgui_event *event);

    void addDeadObject(Widget *widget);

    void run(void);

    struct rtgui_app *getApplication(void)
    {
        return application;
    }

    static Application *getApplicationSelf();

    void activate(void)
    {
        rtgui_app_activate(application);
    }

    void syncTimerPut(SyncTimer *timer);
    void syncTimerRemove(SyncTimer *timer);

    Window* window()
    {
        return m_window;
    }
    static const char* version();

protected:
    void setWindow(Window *window)
    {
        m_window = window;
    }

private:
    void destoryDeadObjects();
    void handleSyncTimer();

private:
    Window *m_window;
    struct rtgui_app *application;
    std::vector<Widget*> m_destory;
    std::list<SyncTimer*> m_syncTimerList;
};

}
