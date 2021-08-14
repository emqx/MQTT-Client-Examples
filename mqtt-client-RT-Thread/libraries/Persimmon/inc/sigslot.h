/*
 * File      : sigslot.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */

#pragma once

namespace Persimmon
{

namespace sigslot_detail
{
class _not_a_param_type;

/* Slots with no parameters. */
class SlotBase0
{
public:
    virtual void exec(void) = 0;
    virtual ~SlotBase0() {};
};

template<typename T>
class SlotImpl0 : public SlotBase0
{
public:
    SlotImpl0(T* pObj, void (T::*func)(void))
    {
        m_obj = pObj;
        m_func = func;
    }

    virtual void exec(void)
    {
        (m_obj->*m_func)();
    }

private:
    T* m_obj;
    void (T::*m_func)(void);
};

class Slot0
{
public:
    template<typename T>
    void set(T* pObj, void (T::*func)(void))
    {
        if (m_slotBase)
            delete m_slotBase;
        m_slotBase = new SlotImpl0<T>(pObj, func);
    }

    Slot0() : m_slotBase(0) {}

    ~Slot0()
    {
        delete m_slotBase;
    }

    void exec(void)
    {
        if (m_slotBase)
            m_slotBase->exec();
    }

private:
    SlotBase0* m_slotBase;

    Slot0(const Slot0& slt) : m_slotBase(slt.m_slotBase)
    {}

    void operator=(const Slot0& slt) {}
};

/* Slots with 1 parameter. */
template<typename Tp1>
class SlotBase1
{
public:
    virtual void exec(Tp1 p1) = 0;
    virtual ~SlotBase1() {};
};

template<typename T, typename Tp1>
class SlotImpl1 : public SlotBase1<Tp1>
{
public:
    SlotImpl1(T* pObj, void (T::*func)(Tp1))
    {
        m_obj = pObj;
        m_func = func;
    }

    virtual void exec(Tp1 p1)
    {
        (m_obj->*m_func)(p1);
    }

private:
    T* m_obj;
    void (T::*m_func)(Tp1);
};

template<typename Tp1>
class Slot1
{
public:
    template<typename T>
    void set(T* pObj, void (T::*func)(Tp1))
    {
        if (m_slotBase)
            delete m_slotBase;
        m_slotBase = new SlotImpl1<T, Tp1>(pObj, func);
    }


    Slot1() : m_slotBase(0) {}

    ~Slot1()
    {
        delete m_slotBase;
    }

    void exec(Tp1 p1)
    {
        if (m_slotBase)
            m_slotBase->exec(p1);
    }

private:
    SlotBase1<Tp1>* m_slotBase;

    Slot1(const Slot1<Tp1>& slt) : m_slotBase(slt.m_slotBase)
    {}

    void operator=(const Slot1<Tp1>& slt)
    {}

};

/* Slots with 2 parameters. */
template<typename Tp1, typename Tp2>
class SlotBase2
{
public:
    virtual void exec(Tp1, Tp2) = 0;
    virtual ~SlotBase2() {};
};

template<typename T, typename Tp1, typename Tp2>
class SlotImpl2 : public SlotBase2<Tp1, Tp2>
{
public:
    SlotImpl2(T* pObj, void (T::*func)(Tp1, Tp2))
    {
        m_obj = pObj;
        m_func = func;
    }

    virtual void exec(Tp1 p1, Tp2 p2)
    {
        (m_obj->*m_func)(p1, p2);
    }

private:
    T* m_obj;
    void (T::*m_func)(Tp1, Tp2);
};

template<typename Tp1, typename Tp2>
class Slot2
{
public:
    template<typename T>
    void set(T* pObj, void (T::*func)(Tp1, Tp2))
    {
        if (m_slotBase)
            delete m_slotBase;
        m_slotBase = new SlotImpl2<T, Tp1, Tp2>(pObj, func);
    }


    Slot2() : m_slotBase(0) {}

    ~Slot2()
    {
        delete m_slotBase;
    }

    void exec(Tp1 p1, Tp2 p2)
    {
        if (m_slotBase)
            m_slotBase->exec(p1, p2);
    }

private:
    SlotBase2<Tp1, Tp2>* m_slotBase;

    Slot2(const Slot2<Tp1, Tp2>& slt) : m_slotBase(slt.m_slotBase)
    {}

    void operator=(const Slot2<Tp1, Tp2>& slt)
    {}

};

}

template<typename Tp1, typename Tp2=sigslot_detail::_not_a_param_type>
class Signal
{
public:
    template<typename T>
    void connect(T* pObj, void (T::*func)(Tp1, Tp2))
    {
        m_slot.set(pObj, func);
    }

    Signal(): m_slot() {}

    ~Signal() {}

    void operator()(Tp1 p1, Tp2 p2)
    {
        m_slot.exec(p1, p2);
    }

private:
    sigslot_detail::Slot2<Tp1, Tp2> m_slot;
};

template<typename Tp1>
class Signal<Tp1, sigslot_detail::_not_a_param_type>
{
public:
    template<typename T>
    void connect(T* pObj, void (T::*func)(Tp1))
    {
        m_slot.set(pObj, func);
    }

    Signal(): m_slot() {}

    ~Signal() {}

    void operator()(Tp1 p1)
    {
        m_slot.exec(p1);
    }

private:
    sigslot_detail::Slot1<Tp1> m_slot;
};

template<>
class Signal<void>
{
public:
    template<typename T>
    void connect(T* pObj, void (T::*func)(void))
    {
        m_slot.set(pObj,func);
    }

    Signal(): m_slot() {}

    ~Signal() {}

    void operator()(void)
    {
        m_slot.exec();
    }

private:
    sigslot_detail::Slot0 m_slot;
};

}
