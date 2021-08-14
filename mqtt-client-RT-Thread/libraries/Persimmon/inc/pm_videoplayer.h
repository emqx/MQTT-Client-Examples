/*
 * File      : pm_videoplayer.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */
#ifndef PM_VEDIOPLAYER_H__
#define PM_VEDIOPLAYER_H__

#include <rtthread.h>

#include <pm_image.h>
#include <pm_widget.h>
#include <sigslot.h>
#include <resource.h>
#include "js_app.h"

namespace Persimmon
{

class VideoPage;

enum videoplayer_rect
{
    none_rect,
    video_rect,
    playBtn_rect,
    fullBtn_rect,
    probar_rect
};

enum video_gesture_type
{
    cancel,
    pause,
    play,
    seek,
    navigate
};

class VideoPlayer : public Widget
{
public:
    VideoPlayer();
    virtual ~VideoPlayer();

    void setPlayImage(Image *imgPlay);
    Image *getPlayImage(void);

    void setPauseImage(Image *imgPause);
    Image *getPauseImage(void);

    void setFullScreenImage(Image *imgFullScreen);
    Image *getFullScreenImage(void);

    void setPrgbarBarImage(Image *imgPrgbarBar);
    Image *getPrgbarBarImage(void);

    void setPrgbarNorImage(Image *imgPrgbarNor);
    Image *getPrgbarNorImage(void);

    void setStatusBgImage(Image *imgStatusBg);
    Image *getStatusBgImage(void);

    void setVolumeImage(Image *imgVolume);
    Image *getVolumeImage(void);

    void setUrl(const char *url);
    const char *getUrl(void);

    void setCurrentTime(int currentTime);
    int getCurrentTime(void);

    void setTotalTime(int totalTime);
    int getTotalTime(void);

    void setIsStretch(bool isStretch);
    bool getIsStretch(void);

    void setStatusHight(int isHideStatus);
    int getStatusHight(void);

    void setIsPlay(bool isPlay);
    bool getIsPlay(void);

    void setIsDraging(bool isDraging);
    bool getIsDraging(void);

    void setIsFullScreen(bool isFullScreen);
    bool getIsFullScreen(void);

    void setVolume(int volume);
    int getVolume(void);

    void setIsHideStatus(bool isHideStatus);
    bool getIsHideStatus(void);

    void setFullPage(VideoPage *fullPage);
    VideoPage *getFullPage(void);

    Rect getScreenInfo(void);
    void setScreenSize(void);

    rt_sem_t getNavigateSem(void);

    void enterVideoPage(void);
    void pauseVideo(void);
    void playVideo(void);
    void seekVideo(int time);
    void stopVideo(void);

    virtual bool handleGestureEvent(struct rtgui_event_gesture *, const struct rtgui_gesture *);
    virtual void render(struct rtgui_dc *dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

    Signal<unsigned int> bindChanged, bindTap;

private:
    VideoPage *fullPage;
    Image *imgPlay, *imgPause, *imgFullScreen, *imgPrgbarBar, *imgPrgbarNor, *imgStatusBg, *imgVolume;
    char *url;
    int pitchGest, currentTime, totalTime, statusHight, currentTapTs, volume;
    bool isPlay, isSetUrl, isStretch, isDraging, isFullScreen, isHideStatus, layer2Shown;
    video_gesture_type gestureType;
    Timer *gestureTimer;
    void handlerGesture(void);
    Widget *getCurrentWin(void);
    videoplayer_rect checkGestRect(const struct rtgui_gesture *gest);
    rt_sem_t navigate_sem;
};

class VideoPage : public Page
{
public:
    VideoPage(Window *win, VideoPlayer *player);
    virtual ~VideoPage();

    void exitVideoPage(void);

    virtual bool handleGestureEvent(struct rtgui_event_gesture *, const struct rtgui_gesture *);
    virtual void renderStatusbar(void);
    virtual void render(struct rtgui_dc *dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(),
                        RenderFlag flags = DrawNormal);

private:
    void handlerGesture(void);
    void shownLayer2(void);
    
    VideoPlayer *player;
    Timer *gestureTimer;
    int currentTapTs, volume, pitchGest;
    video_gesture_type gestureType;
    bool layer2Shown;
    struct rtgui_dc *statusbarDc;
};

}

#endif
