#pragma once

//#include <js_object.h>
#include <pm_page.h>
#include <js_window.h>

#include <string>
#include <map>

class JsObject;
typedef std::map<std::string, jerry_value_t> Listener;

class JsPage : public JsObject, public Persimmon::Page
{
public:
    JsPage(JsWindow *win, jerry_value_t obj, std::string &xml);
    virtual ~JsPage();

    void onLoad(jerry_value_t obj);
    void onResume(void);
    void onExit(void);
    void onUpdate(jerry_value_t obj);
    virtual void onShowPage(void);
    virtual void onHidePage(void);
    virtual void initJsObject(void);
    virtual void setData(void* data);
    void setData(jerry_value_t data);
    static bool isJsPage(jerry_value_t page);

    void setExitAnimation(enum AnimType type, rt_uint16_t frames, rt_uint16_t frameInterval)
    {
        exitAnimType = type;
        exitAnimFrames = frames;
        exitAnimFrameInterval = frameInterval;
    }

    void updateExitAnim(void)
    {
        if (exitAnimType != AnimNone)
        {
            setAnimation(exitAnimType, exitAnimFrames, exitAnimFrameInterval, true);
        }
    }

    virtual void prerender();
    void requestPrerender(jerry_value_t func);
    void addListener(jerry_value_t name, jerry_value_t func);
    bool callListener(const char *name, jerry_value_t value);
    void setJsFunction(jerry_value_t object, const char* func);
    void bindTouch(const char *type, const struct rtgui_gesture *gest);
    virtual bool handleGestureEvent(struct rtgui_event_gesture *gev, const struct rtgui_gesture *gest);

private:
    void clearPrerender();
    void clearListener();

private:
    typedef std::map<jerry_value_t, int> PrerenderMap;
    enum AnimType exitAnimType;
    rt_uint16_t exitAnimFrames, exitAnimFrameInterval;
    jerry_value_t jsCallFunc;
    jerry_value_t jsObj;
    struct rtgui_dc *provisionalBg;
    struct rtgui_gesture lastGest;
    bool isOnExit, isOnShow;  //already call OnExit
    PrerenderMap m_prerenderMap;
    int firstShow;
    Listener listener;
};
