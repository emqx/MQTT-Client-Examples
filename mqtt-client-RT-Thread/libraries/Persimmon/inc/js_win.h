
#pragma once

#include <pm_window.h>
#include <pm_timer.h>

using namespace Persimmon;

class LogoWin : public Window
{
public:
    LogoWin(int timeMs);
    virtual ~LogoWin();

    void timeout(void);
    virtual rt_bool_t dealCmd(struct rtgui_event_command *cmd);
/*#ifdef PKG_JS_PERSIMMON_DEVELOPMENT_MODE
    virtual bool handleGestureEvent(struct rtgui_event_gesture *gev, const struct rtgui_gesture *gest);
#endif*/
    virtual void render(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

private:
    Image *logoImage;
    struct rtgui_image *img;
    Timer *timer;
};
