/*
 * File      : pm_window.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */

#pragma once

#include <rtgui/widgets/window.h>

#include <pm_widget.h>
#include <pm_container.h>
#include <vector>
#include <pm_page.h>

namespace Persimmon
{

class Page;

template<typename T> class ValueAnimator;

class Window : public Container
{
    typedef Container super;
public:
    enum AnimType
    {
        AnimNone = 0x00,
        AnimFade = 0x01,
        AnimMove = 0x02,
        AnimMoveTop = 0x04,
        AnimMoveBottom = 0x08,
        AnimMoveLeft = 0x10,
        AnimMoveRight = 0x20,
    };
    DEFINE_CLASS_ENUM_FLAG_OPERATORS(AnimType);

    /* create a main window */
    Window(const char *title);
    /* create a normal window */
    Window(struct rtgui_win *parent, const char *title, rtgui_rect_t *rect, rt_uint16_t style);

    virtual ~Window();

    virtual int show(rt_bool_t isModal = RT_FALSE);
    virtual void close(int code = 0);
    void hide()
    {
        rtgui_win_hide(getWindow());
    }

    void move(int x, int y);

    struct rtgui_win* getWindow(void)
    {
        return RTGUI_WIN(widget);
    }
    Widget *getChildbyName(const char *name);

    rt_bool_t privateEventHandler(struct rtgui_event *event);
    virtual rt_bool_t eventHandler(struct rtgui_event *event);
    virtual rt_bool_t dealKbd(struct rtgui_event_kbd *kev);
    virtual rt_bool_t dealCmd(struct rtgui_event_command *cmd);

    void saveClip(struct rtgui_region &region);
    void restoreClip(struct rtgui_region &region);

    void addFloatingWidget(Widget *widget);
    Widget *getFloatingWidgetbyName(const char *name);
    void removeFloatingWidget(Widget *widget);
    void renderFloatingWidget(struct rtgui_dc *dc, rtgui_rect_t *rect);
    void renderLogo(struct rtgui_dc *dc, rtgui_rect_t *rect);

    /* Animation */
    void setAnimType(enum AnimType type, Point start = Point(0, 0), Point end = Point(0, 0));
    void setAnimFrame(rt_uint16_t frames, rt_uint16_t frameInterval = 50);  //frameInterval(ms)
    void setAnimDc(struct rtgui_dc *fgDc, struct rtgui_dc *bgDc);
    void setAnimFade(bool fadeOut, rt_uint8_t min = 0, rt_uint8_t max = 255);
    void doAnimation(Widget *widget, bool show = true);
    void setAnimation(enum AnimType type, rt_uint16_t frames, rt_uint16_t frameInterval, bool fadeOut = false);
    void doAnimShow(Page *page = RT_NULL);
    void doAnimHide(Page *page = RT_NULL);

    void paintWindow(void);
    void clearGesture(void);
    void clearWidget(Widget *widget);

    Signal<void> signalShow;

    /* Page manage API */
    void redirectTo(Page *page);
    void navigateTo(Page *page, int num = 0);
    void navigateBack(bool update = true, int depth = 1);
    void slideTo(Page *page, const Point &pos, bool moveOut = false);
    void emptyPage(void);
    Page* getCurrentPage(void);
    Page* getLastPage(unsigned int index = 0);
    Page* getPagebyName(const char *name);
    const std::vector<Page*>& getPages() const
    {
        return m_pages;
    }
    Page* closePage(const char* name);
    bool checkPage(Page *page);
    void showPages(void);
    unsigned int getPageSize(void);
    virtual void pageEmpty(void);

    virtual void render(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

protected:
    void paintChildren();
    rt_bool_t dealMouseButton(struct rtgui_event_mouse *mev);
    rt_bool_t dealMouseMotion(struct rtgui_event_mouse *mev);
    rt_bool_t dealGesture(struct rtgui_event_gesture *gev);
    rt_bool_t gestCancel(struct rtgui_event_gesture *gev, Widget *owner);

private:
    void setupMouseOwner(void);
    Widget *setupMouseOwnerFloating(void);
    void renderFloatingWidget(struct rtgui_dc *dc);
    void renderLogo(rtgui_rect_t *rect);
    void onAnimShowFinished();
    void onAnimHideFinished();
    Page* popPages(int depth);
    void prerender();
    void deleteHidePages();

private:
    struct rtgui_gesture gesture;
    enum rtgui_gesture_type gestType;
    Widget *mouseEventOwner;
    rt_uint32_t curMouseId;
    bool RTGUI_MOUSE_BUTTON_IS_DOWN, SHOW_FIRST;

    /* floating widget */
    std::vector<Widget*> childrenFloating;

    /* Animation */
    enum AnimType animType;
    Point animStart, animEnd;
    bool doAnim, animFadeOut;
    rt_uint8_t animFadeMin, animFadeMax;
    rt_uint16_t animFrames, animFrameInterval;
    struct rtgui_dc *animFgDc, *animBgDc;

    ValueAnimator<Point> *m_animMove;
    ValueAnimator<int> *m_animFade;

    /* page */
    friend class Page;
    std::vector<Page*> m_pages, m_deletePages;
    Page *m_animPage;
    bool firstPageShow;
};

}
