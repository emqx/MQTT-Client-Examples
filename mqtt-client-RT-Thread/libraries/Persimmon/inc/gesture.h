/*
 * File      : gesture.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */

#ifndef _RT_GESTURE_H_
#define _RT_GESTURE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <rtgui/event.h>

struct rtgui_gesture
{
    struct rtgui_object *owner;
    rt_uint32_t touch_id;

    enum rtgui_gesture_type interest;

    /* Public info. Read only for application and recognizer. */
    struct
    {
        /* Pixel per second. */
        int x, y;
    } speed;
    /* FIXME: Need acceleration? */

    /* @last will behold while recognizing the gesture. It should never be
     * updated by the recognizer. The @current is the most recent point and
     * timestamp is read-only to the recognizer too. These members will be
     * updated by the gesture framwork. */
    struct
    {
        rt_uint16_t x, y;
        rt_tick_t ts;
    } start, last, current, speeds;

    /* Private info. Read/write for recognizer. */
    struct rtgui_timer *timer;
    rt_uint32_t start_stat;
    rt_uint32_t finish_stat;
};

int rtgui_gesture_init(struct rtgui_gesture *gest,
                       struct rtgui_object *owner,
                       enum rtgui_gesture_type inter);
void rtgui_gesture_cleanup(struct rtgui_gesture *gest);

/** Try to recognize gestures from the @eve.
 *
 * @eve should be a mouse event(either RTGUI_EVENT_MOUSE_BUTTON or
 * RTGUI_EVENT_MOUSE_MOTION) and every mouse event should be feed into this
 * function to get a possible gesture.
 *
 * When a gesture is recognized, it will return RT_TRUE. Otherwise, it will
 * return RT_FALSE. The event handler of the @owner will be called once a
 * gesture is recognized, with a rtgui_event_gesture event.
 */
rt_bool_t rtgui_gesture_recognize(struct rtgui_gesture *gest,
                                  const struct rtgui_event *eve);

void set_gesture_longtap_duration(rt_uint32_t tick);
void set_gesture_drag_start(rt_uint16_t pixel_size);
void set_gesture_drag_gap(rt_uint16_t pixel_size);
void set_gesture_tick_precision(rt_uint32_t tick);

#ifdef __cplusplus
}
#endif

#endif /* _RT_GESTURE_H_ */
