/**
  ******************************************************************************
  * This file is part of the TouchGFX 4.15.0 distribution.
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/**
 * @file touchgfx/Callback.hpp
 *
 * Declares the touchgfx::GenericCallback and touchgfx::Callback classes.
 */
#ifndef CALLBACK_HPP
#define CALLBACK_HPP

namespace touchgfx
{
/**
 * GenericCallback is the base class for callbacks.
 *
 * The reason this base class exists, is that a normal Callback requires the class type
 * where the callback function resides to be known. This is problematic for ie.
 * framework widgets like AbstractButton, on which it should be possible to register a
 * callback on object types that are user-specific and thus unknown to AbstractButton.
 * This is solved by having AbstractButton contain a pointer to a GenericCallback
 * instead. This pointer must then be initialized to point on an instance of Callback,
 * created by the user, which is initialized with the appropriate object type.
 *
 * @tparam T1 The type of the first argument in the member function, or void if none.
 * @tparam T2 The type of the second argument in the member function, or void if none.
 * @tparam T3 The type of the third argument in the member function, or void if none.
 *
 * @see Callback
 *
 * @note As with Callback, this class exists in four versions to support callback functions
 *       taking zero, one, two or three arguments.
 */
template <class T1 = void, class T2 = void, class T3 = void>
class GenericCallback
{
public:
    /** Finalizes an instance of the GenericCallback class. */
    virtual ~GenericCallback()
    {
    }

    /**
     * Calls the member function. Do not call execute unless isValid() returns true (ie. a
     * pointer to the object and the function has been set).
     *
     * @param  val1 This value will be passed as the first argument in the function call.
     * @param  val2 This value will be passed as the second argument in the function call.
     * @param  val3 This value will be passed as the third argument in the function call.
     */
    virtual void execute(T1 val1, T2 val2, T3 val3) = 0;

    /**
     * Function to check whether the Callback has been initialized with values.
     *
     * @return true If the callback is valid (i.e. safe to call execute).
     */
    virtual bool isValid() const = 0;
};

/**
 * GenericCallback is the base class for callbacks.
 *
 * The reason this base class exists, is that a normal Callback requires the class type
 * where the callback function resides to be known. This is problematic for ie.
 * framework widgets like AbstractButton, on which it should be possible to register a
 * callback on object types that are user-specific and thus unknown to AbstractButton.
 * This is solved by having AbstractButton contain a pointer to a GenericCallback
 * instead. This pointer must then be initialized to point on an instance of Callback,
 * created by the user, which is initialized with the appropriate object type.
 *
 * @tparam T1 The type of the first argument in the member function, or void if none.
 * @tparam T2 The type of the second argument in the member function, or void if none.
 *
 * @see Callback
 *
 * @note As with Callback, this class exists in four versions to support callback functions
 *       taking zero, one, two or three arguments.
 */
template <class T1, class T2>
class GenericCallback<T1, T2, void>
{
public:
    /** Finalizes an instance of the void> class. */
    virtual ~GenericCallback()
    {
    }

    /**
     * Calls the member function. Do not call execute unless isValid() returns true (ie. a
     * pointer to the object and the function has been set).
     *
     * @param  val1 This value will be passed as the first argument in the function call.
     * @param  val2 This value will be passed as the second argument in the function call.
     */
    virtual void execute(T1 val1, T2 val2) = 0;

    /**
     * Function to check whether the Callback has been initialized with values.
     *
     * @return true If the callback is valid (i.e. safe to call execute).
     */
    virtual bool isValid() const = 0;
};

/**
 * GenericCallback is the base class for callbacks.
 *
 * The reason this base class exists, is that a normal Callback requires the class type
 * where the callback function resides to be known. This is problematic for ie.
 * framework widgets like AbstractButton, on which it should be possible to register a
 * callback on object types that are user-specific and thus unknown to AbstractButton.
 * This is solved by having AbstractButton contain a pointer to a GenericCallback
 * instead. This pointer must then be initialized to point on an instance of Callback,
 * created by the user, which is initialized with the appropriate object type.
 *
 * @tparam T1 The type of the first argument in the member function, or void if none.
 *
 * @see Callback
 *
 * @note As with Callback, this class exists in four versions to support callback functions
 *       taking zero, one, two or three arguments.
 */
template <class T1>
class GenericCallback<T1, void, void>
{
public:
    /** Finalizes an instance of the void> class. */
    virtual ~GenericCallback()
    {
    }

    /**
     * Calls the member function. Do not call execute unless isValid() returns true (ie. a
     * pointer to the object and the function has been set).
     *
     * @param  val1 This value will be passed as the first argument in the function call.
     */
    virtual void execute(T1 val1) = 0;

    /**
     * Function to check whether the Callback has been initialized with values.
     *
     * @return true If the callback is valid (i.e. safe to call execute).
     */
    virtual bool isValid() const = 0;
};

/**
 * GenericCallback is the base class for callbacks.
 *
 * The reason this base class exists, is that a normal Callback requires the class type
 * where the callback function resides to be known. This is problematic for ie.
 * framework widgets like AbstractButton, on which it should be possible to register a
 * callback on object types that are user-specific and thus unknown to AbstractButton.
 * This is solved by having AbstractButton contain a pointer to a GenericCallback
 * instead. This pointer must then be initialized to point on an instance of Callback,
 * created by the user, which is initialized with the appropriate object type.
 *
 * @see Callback
 *
 * @note As with Callback, this class exists in four versions to support callback functions
 *       taking zero, one, two or three arguments.
 */
template <>
class GenericCallback<void>
{
public:
    /** Finalizes an instance of the GenericCallback<void> class. */
    virtual ~GenericCallback()
    {
    }

    /**
     * Calls the member function. Do not call execute unless isValid() returns true (ie. a
     * pointer to the object and the function has been set).
     */
    virtual void execute() = 0;

    /**
     * Function to check whether the Callback has been initialized with values.
     *
     * @return true If the callback is valid (i.e. safe to call execute).
     */
    virtual bool isValid() const = 0;
};

/**
 * A Callback is basically a wrapper of a pointer-to-member-function.
 *
 * It is used for registering callbacks between widgets. For instance, a Button can be
 * configured to call a member function when it is clicked.
 *
 * The class is templated in order to provide the class type of the object in which the
 * member function resides, and the argument types of the function to call.
 *
 * The Callback class exists in four versions, for supporting member functions with 0,
 * 1, 2 or 3 arguments. The compiler will infer which type to use automatically.
 *
 * @tparam dest_type The type of the class in which the member function resides.
 * @tparam T1        The type of the first argument in the member function, or void if none.
 * @tparam T2        The type of the second argument in the member function, or void if none.
 * @tparam T3        The type of the third argument in the member function, or void if none.
 *
 * @note The member function to call must return void. The function can have zero, one, two or
 *       three arguments of any type.
 */
template <class dest_type, typename T1 = void, typename T2 = void, typename T3 = void>
struct Callback : public GenericCallback<T1, T2, T3>
{
    /** Initializes a new instance of the Callback class. */
    Callback()
        : pobject(0), pmemfun_3(0)
    {
    }

    /**
     * Initializes a Callback with an object and a pointer to the member function in that
     * object to call.
     *
     * Initializes a Callback with an object and a pointer to the member function in that
     * object to call.
     *
     * @param [in] pobject   Pointer to the object on which the function should be called.
     * @param [in] pmemfun_3 Address of member function. This is the version where function takes
     *                       three arguments.
     */
    Callback(dest_type* pobject, void (dest_type::*pmemfun_3)(T1, T2, T3))
    {
        this->pobject = pobject;
        this->pmemfun_3 = pmemfun_3;
    }

    /**
     * Calls the member function. Do not call execute unless isValid() returns true (ie. a
     * pointer to the object and the function has been set).
     *
     * @param  t1 This value will be passed as the first argument in the function call.
     * @param  t2 This value will be passed as the second argument in the function call.
     * @param  t3 This value will be passed as the third argument in the function call.
     */
    virtual void execute(T1 t1, T2 t2, T3 t3)
    {
        (pobject->*pmemfun_3)(t1, t2, t3);
    }

    /**
     * Function to check whether the Callback has been initialized with values.
     *
     * @return true If the callback is valid (i.e. safe to call execute).
     */
    virtual bool isValid() const
    {
        return (pobject != 0) && (pmemfun_3 != 0);
    }

private:
    dest_type* pobject;
    void (dest_type::*pmemfun_3)(T1, T2, T3);
};

/**
 * A Callback is basically a wrapper of a pointer-to-member-function.
 *
 * It is used for registering callbacks between widgets. For instance, a Button can be
 * configured to call a member function when it is clicked.
 *
 * The class is templated in order to provide the class type of the object in which the
 * member function resides, and the argument types of the function to call.
 *
 * The Callback class exists in four versions, for supporting member functions with 0, 1,
 * 2 or 3 arguments. The compiler will infer which type to use automatically.
 *
 * @tparam dest_type The type of the class in which the member function resides.
 * @tparam T1        The type of the first argument in the member function, or void if none.
 * @tparam T2        The type of the second argument in the member function, or void if none.
 *
 * @note The member function to call must return void. The function can have zero, one, two or
 *       three arguments of any type.
 */
template <class dest_type, typename T1, typename T2>
struct Callback<dest_type, T1, T2, void> : public GenericCallback<T1, T2>
{
    /** Initializes a new instance of the Callback class. */
    Callback()
        : pobject(0), pmemfun_2(0)
    {
    }

    /**
     * Initializes a Callback with an object and a pointer to the member function in that
     * object to call.
     *
     * @param [in] pobject   Pointer to the object on which the function should be called.
     * @param [in] pmemfun_2 Address of member function. This is the version where function takes
     *                       two arguments.
     */
    Callback(dest_type* pobject, void (dest_type::*pmemfun_2)(T1, T2))
    {
        this->pobject = pobject;
        this->pmemfun_2 = pmemfun_2;
    }

    /**
     * Calls the member function. Do not call execute unless isValid() returns true (ie. a
     * pointer to the object and the function has been set).
     *
     * @param  t1 This value will be passed as the first argument in the function call.
     * @param  t2 This value will be passed as the second argument in the function call.
     */
    virtual void execute(T1 t1, T2 t2)
    {
        (pobject->*pmemfun_2)(t1, t2);
    }

    /**
     * Function to check whether the Callback has been initialized with values.
     *
     * @return true If the callback is valid (i.e. safe to call execute).
     */
    virtual bool isValid() const
    {
        return (pobject != 0) && (pmemfun_2 != 0);
    }

private:
    dest_type* pobject;
    void (dest_type::*pmemfun_2)(T1, T2);
};

/**
 * A Callback is basically a wrapper of a pointer-to-member-function.
 *
 * It is used for registering callbacks between widgets. For instance, a Button can be
 * configured to call a member function when it is clicked.
 *
 * The class is templated in order to provide the class type of the object in which the
 * member function resides, and the argument types of the function to call.
 *
 * The Callback class exists in four versions, for supporting member functions with 0, 1,
 * 2 or 3 arguments. The compiler will infer which type to use automatically.
 *
 * @tparam dest_type The type of the class in which the member function resides.
 * @tparam T1        The type of the first argument in the member function, or void if none.
 *
 * @note The member function to call must return void. The function can have zero, one, two or
 *       three arguments of any type.
 */
template <class dest_type, typename T1>
struct Callback<dest_type, T1, void, void> : public GenericCallback<T1>
{
    /** Initializes a new instance of the Callback class. */
    Callback()
        : pobject(0), pmemfun_1(0)
    {
    }

    /**
     * Initializes a Callback with an object and a pointer to the member function in that
     * object to call.
     *
     * @param [in] pobject   Pointer to the object on which the function should be called.
     * @param [in] pmemfun_1 Address of member function. This is the version where function takes
     *                       one argument.
     */
    Callback(dest_type* pobject, void (dest_type::*pmemfun_1)(T1))
    {
        this->pobject = pobject;
        this->pmemfun_1 = pmemfun_1;
    }

    /**
     * Calls the member function. Do not call execute unless isValid() returns true (ie. a
     * pointer to the object and the function has been set).
     *
     * @param  t1 This value will be passed as the first argument in the function call.
     *
     * @see isValid
     */
    virtual void execute(T1 t1)
    {
        (pobject->*pmemfun_1)(t1);
    }

    /**
     * Query if this object is valid.
     *
     * @return true if valid, false if not.
     */
    virtual bool isValid() const
    {
        return (pobject != 0) && (pmemfun_1 != 0);
    }

private:
    dest_type* pobject;
    void (dest_type::*pmemfun_1)(T1);
};

/**
 * A Callback is basically a wrapper of a pointer-to-member-function.
 *
 * It is used for registering callbacks between widgets. For instance, a Button can be
 * configured to call a member function when it is clicked.
 *
 * The class is templated in order to provide the class type of the object in which the
 * member function resides, and the argument types of the function to call.
 *
 * The Callback class exists in four versions, for supporting member functions with 0, 1,
 * 2 or 3 arguments. The compiler will infer which type to use automatically.
 *
 * @tparam dest_type The type of the class in which the member function resides.
 *
 * @note The member function to call must return void. The function can have zero, one, two or
 *       three arguments of any type.
 */
template <class dest_type>
struct Callback<dest_type, void, void, void> : public GenericCallback<>
{
    /** Initializes a new instance of the Callback class. */
    Callback()
        : pobject(0), pmemfun_0(0)
    {
    }

    /**
     * Initializes a Callback with an object and a pointer to the member function in that
     * object to call.
     *
     * @param [in] pobject   Pointer to the object on which the function should be called.
     * @param [in] pmemfun_0 Address of member function. This is the version where function takes
     *                       zero arguments.
     */
    Callback(dest_type* pobject, void (dest_type::*pmemfun_0)())
    {
        this->pobject = pobject;
        this->pmemfun_0 = pmemfun_0;
    }

    /**
     * Calls the member function. Do not call execute unless isValid() returns true (ie. a
     * pointer to the object and the function has been set).
     */
    virtual void execute()
    {
        (pobject->*pmemfun_0)();
    }

    /**
     * Function to check whether the Callback has been initialized with values.
     *
     * @return true If the callback is valid (i.e. safe to call execute).
     */
    virtual bool isValid() const
    {
        return (pobject != 0) && (pmemfun_0 != 0);
    }

private:
    dest_type* pobject;
    void (dest_type::*pmemfun_0)();
};

} // namespace touchgfx

#endif // CALLBACK_HPP
