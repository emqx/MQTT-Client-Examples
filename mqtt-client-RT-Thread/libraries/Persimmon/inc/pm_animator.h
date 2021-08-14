/*
 * File      : pm_animator.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-06-01     realthread   the first version
 */

#ifndef __PM_ANIMATOR_H__
#define __PM_ANIMATOR_H__

#include <pm_rect.h>
#include <sigslot.h>
#include <vector>

namespace Persimmon
{

class Widget;
class Timer;
class SyncTimer;

class AbstractInterpolator
{
public:
    AbstractInterpolator() {}
    virtual ~AbstractInterpolator() {}
    virtual float interpolation(float input) = 0;
};

/* produce uniform animation */
class LinearInterpolator : public AbstractInterpolator
{
public:
    virtual float interpolation(float input);
};

/* produce accelerate animation */
class AccelerateInterpolator : public AbstractInterpolator
{
public:
    AccelerateInterpolator(float acce = 1.0f);
    virtual float interpolation(float input);

private:
    float m_acce;
};

/* produce slow down animation */
class NegExpInterpolator : public AbstractInterpolator
{
public:
    virtual float interpolation(float input);
};

/* produce bounce animation */
class BounceInterpolator : public AbstractInterpolator
{
public:
    virtual float interpolation(float x);

private:
    float bounce(float t);
};

class AnticipateInterpolator : public AbstractInterpolator
{
public:
    AnticipateInterpolator(float time = 1.0f);
    virtual float interpolation(float x);

private:
    float m_time;
};

class AbstractEvaluator
{
public:
    AbstractEvaluator() {}
    virtual ~AbstractEvaluator() {}
};

template<typename T>
class TypeEvaluator : public AbstractEvaluator
{
public:
    virtual T evaluate(float progress, const T &start, const T &end)
    {
        return end;
    }
};

class RectEvaluator : public TypeEvaluator<Rect>
{
public:
    virtual Rect evaluate(float progress, const Rect &start, const Rect &end);
};

class PointEvaluator : public TypeEvaluator<Point>
{
public:
    virtual Point evaluate(float progress, const Point &start, const Point &end);
};

class IntEvaluator : public TypeEvaluator<int>
{
public:
    virtual int evaluate(float progress, const int &start, const int &end);
};

class FloatEvaluator : public TypeEvaluator<float>
{
public:
    virtual float evaluate(float progress, const float &start, const float &end);
};

class ColorEvaluator : public TypeEvaluator<rtgui_color_t>
{
public:
    virtual rtgui_color_t evaluate(float progress, const rtgui_color_t &start, const rtgui_color_t &end);
};

class AbstractAnimator
{
    friend class AnimatorReducer;
    friend class ParallelAnimator;
public:
    AbstractAnimator();
    virtual ~AbstractAnimator();
    void setDuration(int xms);
    void setInterval(int xms);
    void setRepeat(int times);
    void setReloadInterval(int xms);
    Signal<void>* finishedSignal();
    Signal<void>* startSignal();
    void start();
    void stop();
    bool isPlay() const
    {
        return m_isPlay;
    }
    bool isOdd() const
    {
        return m_isOdd;
    }
    void setOdd(bool isOdd);

protected:
    virtual void playFrame(int index, int total);

private:
    void onTimeout();

private:
    unsigned int m_repeatCount, m_repeat;
    unsigned int m_reloadInterval, m_duration;
    SyncTimer *m_timer;
    bool m_repeatFlag;
    Signal<void> m_finished;
    Signal<void> m_firstFrame;
    unsigned long m_tick, m_tickStart, m_tickEnd;
    bool m_isOdd, m_isPlay;
};

class PropertyAnimator : public AbstractAnimator
{
public:
    enum EaseType
    {
        EaseIn,
        EaseOut,
        EaseInOut
    };
    enum Direction
    {
        Normal,
        Reverse,
        Alternate,
        AlternateReverse
    };
    PropertyAnimator();
    virtual ~PropertyAnimator();
    void setInterpolator(AbstractInterpolator *interpolator);
    void setEvaluator(AbstractEvaluator *evaluate);
    void setEaseType(EaseType type)
    {
        m_easeType = type;
    }
    void setDirection(Direction direction)
    {
        m_direction = direction;
    }
    EaseType easeType() const
    {
        return m_easeType;
    }
    Direction direction() const
    {
        return m_direction;
    }

protected:
    virtual void playFrame(int index, int total);
    virtual void playFrame(float progress) = 0;
    virtual void initialization() {}
    AbstractEvaluator* evaluator();

private:
    float interpolation(float input);

private:
    AbstractInterpolator *m_interpolator;
    AbstractEvaluator *m_evaluator;
    EaseType m_easeType;
    Direction m_direction;
};

template<typename T>
class PropertySettingBase
{
public:
    virtual void setProperty(const T &value) = 0;
    virtual T objectValue() const = 0;
};

template<class T1, typename T2>
class PropertySetting : public PropertySettingBase<T2>
{
public:
    PropertySetting(T1 *obj, void(T1::*func)(const T2 &)) : m_obj(obj), m_func1(func), m_func2(NULL) {}
    PropertySetting(T1 *obj, void(T1::*func)(T2)) : m_obj(obj), m_func1(NULL), m_func2(func) {}

    void setObjectValueGetter(T2(T1::*func)() const)
    {
        m_valueGetter = func;
    }

    virtual void setProperty(const T2 &value)
    {
        if (m_func1) (m_obj->*m_func1)(value);
        else (m_obj->*m_func2)(value);
    }

    virtual T2 objectValue() const
    {
        if (m_valueGetter)
            return (m_obj->*m_valueGetter)();
        return T2();
    }

private:
    T1 *m_obj;
    void(T1::*m_func1)(const T2 &);
    void(T1::*m_func2)(T2);
    T2(T1::*m_valueGetter)() const;
};

template<typename T>
class ValueAnimator : public PropertyAnimator
{
public:
    template<class T1>
    ValueAnimator(T1 *obj, void(T1::*func)(const T &), T(T1::*getter)() const = NULL) : PropertyAnimator()
    {
        PropertySetting<T1, T> *prop = new PropertySetting<T1, T>(obj, func);
        setEvaluator(new TypeEvaluator<T>());
        prop->setObjectValueGetter(getter);
        m_propSet = prop;
        m_relative = getter != NULL;
    }
    template<class T1>
    ValueAnimator(T1 *obj, void(T1::*func)(T), T(T1::*getter)() const = NULL) : PropertyAnimator()
    {
        PropertySetting<T1, T> *prop = new PropertySetting<T1, T>(obj, func);
        setEvaluator(new TypeEvaluator<T>());
        prop->setObjectValueGetter(getter);
        m_propSet = prop;
        m_relative = getter != NULL;
    }
    virtual ~ValueAnimator()
    {
        delete m_propSet;
    }
    T startValue() const
    {
        return m_startValue;
    }
    T endValue() const
    {
        return m_endValue;
    }
    void setStartValue(const T &value)
    {
        m_startValue = value;
    }
    void setEndValue(const T &value)
    {
        m_endValue = value;
    }
    void setValueLimits(const T &start, const T &end)
    {
        m_startValue = start;
        m_endValue = end;
    }
    T objectValue() const
    {
        return m_propSet->objectValue();
    }

protected:
    virtual bool isRelative() const
    {
        return m_relative;
    }
    using PropertyAnimator::playFrame;
    virtual void playFrame(float progress)
    {
        TypeEvaluator<T> *eval = static_cast<TypeEvaluator<T>*>(evaluator());
        T value = eval->evaluate(progress, m_startValue, m_endValue);
        m_propSet->setProperty(value);
    }

private:
    T m_startValue, m_endValue;
    PropertySettingBase<T> *m_propSet;
    bool m_relative;
};

class RectAnimator : public ValueAnimator<Rect>
{
public:
    template<class T>
    RectAnimator(T *obj, void(T::*func)(const Rect &)) : ValueAnimator<Rect>(obj, func)
    {
        setEvaluator(new RectEvaluator());
    }
};

class PointAnimator : public ValueAnimator<Point>
{
public:
    template<class T>
    PointAnimator(T *obj, void(T::*func)(const Point &)) : ValueAnimator<Point>(obj, func)
    {
        setEvaluator(new PointEvaluator());
    }
};

class IntAnimator : public ValueAnimator<int>
{
public:
    template<class T>
    IntAnimator(T *obj, void(T::*func)(int), int(T::*getter)() const = NULL) : ValueAnimator<int>(obj, func, getter)
    {
        setEvaluator(new IntEvaluator());
    }

protected:
    virtual void initialization();
};

class FloatAnimator : public ValueAnimator<float>
{
public:
    template<class T>
    FloatAnimator(T *obj, void(T::*func)(float), float(T::*getter)() const = NULL) : ValueAnimator<float>(obj, func, getter)
    {
        setEvaluator(new FloatEvaluator());
    }
};

class ColorAnimator : public ValueAnimator<rtgui_color_t>
{
public:
    template<class T>
    ColorAnimator(T *obj, void(T::*func)(rtgui_color_t)) : ValueAnimator<rtgui_color_t>(obj, func)
    {
        setEvaluator(new ColorEvaluator());
    }
};

class ParallelAnimator : public AbstractAnimator
{
public:
    ParallelAnimator();
    virtual ~ParallelAnimator();
    void addAnimator(AbstractAnimator *anim);

    virtual void playFrame(int index, int total);

private:
    std::vector<AbstractAnimator *> m_anims;
};

class AnimatorGroup
{
public:
    AnimatorGroup(bool sequence = false, int repeat = 1, bool sync = true);
    ~AnimatorGroup();
    void addAnimator(AbstractAnimator *anim);
    void addAnimator(AnimatorGroup *group);
    void setDuration(int xms, int index = -1);
    void setReloadInterval(int xms, int index = -1);
    void setRepeat(int count);
    void start();
    void reverse();
    void stop();
    Signal<void>* finishedSignal();
    Signal<void>* finishedSignal(int index);

    bool isPlay() const
    {
        return m_isPlay;
    }

private:
    struct AnimatorNode
    {
        bool isGroup;
        AbstractAnimator *anim;
        AnimatorGroup *group;
        int interval;
        Signal<void> *finished;
    };
    void startOneNode(AnimatorNode &node);
    void playNext();
    void finishedOne();
    void restart();
    void checkFinished();

private:
    std::vector<AnimatorNode> m_anims;
    bool m_sequence, m_isPlay, m_reverse;
    int m_playIndex, m_repeat, m_playCount;
    Signal<void> m_finished;
    Timer *m_timer;
};

};

#endif
