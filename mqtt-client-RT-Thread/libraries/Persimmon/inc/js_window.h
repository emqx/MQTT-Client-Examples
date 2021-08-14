#pragma once

#include <js_object.h>
#include <pm_window.h>
#include <pm_timer.h>

using namespace Persimmon;

class JsPage;

namespace Persimmon {
    class IntAnimator;
};

class JsWindow : public JsObject, public Persimmon::Window
{
public:
    JsWindow(const char* name);
    virtual ~JsWindow();

    void setPullDownPage(JsPage *page);
    void onUpdate(jerry_value_t obj);
    virtual rt_bool_t dealCmd(struct rtgui_event_command *cmd);
    void keyboard(jerry_value_t obj);

    void timeout(void);
#ifdef PKG_JS_PERSIMMON_DEVELOPMENT_MODE
    void timeoutEnv(void);
#endif
    void doDelPage(JsPage *page);
    virtual bool handleGestureEvent(struct rtgui_event_gesture *gev, const struct rtgui_gesture *gest);

private:
    void movePullDown(int y);

private:
    Timer *timer;
#ifdef PKG_JS_PERSIMMON_DEVELOPMENT_MODE
    Timer *timerEnv;
#endif
    JsPage *delPage, *pullDownPage;
    int pullDownState;
    IntAnimator *m_anim;
};

