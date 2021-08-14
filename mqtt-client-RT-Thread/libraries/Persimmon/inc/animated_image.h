/*
 * File      : animated_image.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-01-22     realthread   the first version
 */

#pragma once

#include <vector>
#include <image_box.h>
#include <pm_timer.h>
#include <resource.h>
#include <sigslot.h>

namespace Persimmon
{

class AnimatedImage : public ImageBox
{
public:
    AnimatedImage(bool cache = false);
    AnimatedImage(unsigned int ms, bool cache = false);
    AnimatedImage(const AnimatedImage &other);
    virtual ~AnimatedImage();

    virtual Widget *depthCopy(void) const;
    void addImage(const char *img);
    void clearImages();

    void startAnimation(void)
    {
        if (m_imagePath.size() <= 0)
            return;

        m_timer->start();
    }

    void stopAnimation(bool end = false);

    void pauseAnimation(void)
    {
        m_timer->stop();
    }

    void setLoop(bool loop)
    {
        m_isLoop = loop;
    }

    bool isLoop(void)
    {
        return m_isLoop;
    }

    void setInterval(unsigned int ms)
    {
        if (ms > 0)
            m_timer->setTimeout(rt_tick_from_millisecond(ms));
    }

    void enableCache(void)
    {
        m_isCache = true;
    }

    virtual void render(struct rtgui_dc *dc, const Point &dcPoint = Point(),
        const Rect &srcRect = Rect(), RenderFlag flags = DrawNormal);

    Signal<int> end;

protected:
    void timeout(void);
    void empty(void);

private:
    void updateImage();

private:
    Timer *m_timer;
    int m_index, m_lastIndex;
    std::vector<char *> m_imagePath;
    std::vector<Image *> m_imageVector;
    Image *m_showImage;
    bool m_isLoop, m_isCache;
};

} // namespace Persimmon
