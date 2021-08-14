/*
 * File      : pm_page.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-03-06     realthread   the first version
 */

#pragma once

#include <pm_window.h>

namespace Persimmon
{

template<typename T> class ValueAnimator;

class Page : public Container
{
public:
    enum Direction
    {
        UnknowDirection = 0,
        Left,
        Right,
        Up,
        Down
    };
    enum MoveType
    {
        Cover,
        Connected
    };
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

    Page(Window *win);
    virtual ~Page();

    void navigateTo(Page *page);
    void navigateBack(bool update = true);
    void navigateBack(unsigned int num, bool update = true);
    void slideTo(Page *next, Direction direction = Down, MoveType moveType = Cover);
    Page* getCurrentPage(void);
    Page *getLastPage(unsigned int index = 0);
    unsigned int getPageSize(void);
    Window* getPageWin(void);

    void setAnimation(enum AnimType type, rt_uint16_t frames, rt_uint16_t frameInterval, bool fadeOut = false);

    virtual bool handleGestureEvent(struct rtgui_event_gesture *gev, const struct rtgui_gesture *gest);

    virtual void prerender();
    virtual void onShowPage(void) {}
    virtual void onHidePage(void) {}

    bool isQuick()
    {
        return m_quick;
    }
    void setQuick(bool quick)
    {
        m_quick = quick;
    }

private:
    void onAnimFinished();
    Page* currentPage();
    Direction reverse(Direction dir);

private:
    Window *pWin;
    Page *m_slideDown, *m_slideUp;
    Direction m_direction;
    MoveType m_moveType;
    ValueAnimator<Point> *m_anim;
    bool m_quick;
};

}
