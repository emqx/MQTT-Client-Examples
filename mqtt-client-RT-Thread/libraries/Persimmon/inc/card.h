/*
 * File      : card.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */

#pragma once

#include <vector>
#include <pm_widget.h>
#include <pm_image.h>
#include <pm_container.h>
#include <pm_timer.h>
#include <sigslot.h>

#ifdef USE_GPU
#include <nema_core.h>
#include <nema_transitions.h>
#endif

namespace Persimmon
{

class AbstractBar;
class IntAnimator;
class AbstractInterpolator;

class Card : public Container
{
    typedef Container super;

public:
    enum type
    {
        HORIZONTAL  = 1 << 0,
        VERTICAL    = 1 << 1,
        CYCLE       = 1 << 2,
    };
    DEFINE_CLASS_ENUM_FLAG_OPERATORS(type);
    enum FixedType
    {
        FixedFirst = 1 << 0,
        FixedLast = 1 << 1,
        FixedCurrent = 1 << 2,
        FixedMask = 7
    };
    enum MoveBufferType
    {
        MoveBufferNone,
        MoveBufferDraw,
        MoveBufferFast,
        MoveBufferHalf,
    };

    Card();
    Card (const Rect& rect, enum type tp = HORIZONTAL);
    virtual ~Card();

    virtual void addChild(Widget* widget);
    virtual void removeChild(Widget* widget);

    void setAnimDuration(int xms);
    void setInterpolator(AbstractInterpolator *interpolator);

    void setDirection(enum type t = HORIZONTAL)
    {
        mtype = t;

        if (mtype & HORIZONTAL)
            setInterestGesture(RTGUI_GESTURE_DRAG_HORIZONTAL);
        else
            setInterestGesture(RTGUI_GESTURE_DRAG_VERTICAL);
    }

    void setDotIndicator(Image *dot_nor, Image *dot_sel)
    {
        if (this->dot_nor)
            delete this->dot_nor;

        if (this->dot_sel)
            delete this->dot_sel;

        this->dot_nor = dot_nor;
        this->dot_sel = dot_sel;
    }

    void setScrollBar(AbstractBar *bar);
    void scrollToCardPage(int index);
    void selectCardPage(int index);
    void setPageSize(int size);
    void setFixed(int fixedType);
    void setMoveBuffer(bool enabled);
    void setMoveBuffer(MoveBufferType type);

    bool isCycle() const;

    void nextCardPage(void)
    {
        scrollToCardPage(m_currentIndex + 1);
    }

    void prevCardPage(void)
    {
        scrollToCardPage(m_currentIndex - 1);
    }

    int getCurrentCardPage() const
    {
        return m_currentIndex;
    }

    int pageCount(void)
    {
        return children.size();
    }

    virtual void render(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);
    virtual bool handleGestureEvent(struct rtgui_event_gesture *gev,
                                    const struct rtgui_gesture *gest);

    //virtual Widget* getMouseOwner(const rtgui_gesture *gesture);

    Signal<int, int> changed;

#ifdef USE_GPU
    void set_gpu_transeffect(nema_transition_t effect);
#endif

protected:
    enum CardEvent
    {
        UnchangedEvent,
        ChangedEvent,
        DropFirstEvent,
        DropLastEvent,
        DropStartEvent,
        DropBeginEvent,
        DropEndEvent,
        UnknowCardEvent
    };
    virtual void moveEvent(CardEvent event, int index);

private:
    void changeCardPage(bool next);
    void renderIndicator(struct rtgui_dc *dc, const Point &point);
    void onAnimRun(int progress);
    void onAnimFinished();
    void fixChildPosition(int pitch);
    void fixViewPosition(int index, int x, int y, bool cycle);
    int gestOffset(const struct rtgui_gesture *gest);
    void updateScrollBar();
    void fixMovePosition(int pitch, int size);
    int patchPosition();
    int gestPitch(const struct rtgui_gesture *gest) const;
    int pageSize() const;
    int cardSize() const;
    void updateBuffer();
    void updateBufferFast();
    void initBufferDraw(int index, int maxIndex);
    void initBufferHalf(int index, int maxIndex);
    void updateBufferDrawOrHalf(bool isDraw);
    void drawHoriBuffer(struct rtgui_dc *dc, int index,int pos, int size, RenderFlag flags);
    void drawVertBuffer(struct rtgui_dc *dc, int index,int pos, int size, RenderFlag flags);

private:
    enum type mtype;
    IntAnimator *m_anim;
    Image *dot_nor, *dot_sel;
    int m_currentIndex, m_pageSize, m_bufferIndex, m_dirPitch;
    int m_cardPosition, m_movePosition, m_animOffset, m_gestPosition;
    AbstractBar *m_scrollBar;
    uint32_t m_flags;
    struct rtgui_dc* m_buffer[2];
    int m_bufferCurIndex;

#ifdef USE_GPU
    enum {
        GPU_STA_NONE        = 0x0000,
        GPU_STA_START       = 0x0001,
        GPU_STA_RUNNING     = 0x0002,
        GPU_STA_CL_DST      = 0x0004,
    };
    float gpu_step;
    int gpu_move;
    unsigned char gpu_start;
    unsigned char gpu_running;
    nema_transition_t trans_effect;
    nema_cmdlist_t gpu_cmdlist;
#endif

};

}
