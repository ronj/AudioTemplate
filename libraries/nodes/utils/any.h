#ifndef ANY_H
#define ANY_H

#include <type_traits>
#include <typeinfo>
#include <utility>

// TODO: incorporate storage policy as in http://www.codeproject.com/Articles/11250/High-Performance-Dynamic-Typing-in-C-using-a-Repla
// TODO: http://stackoverflow.com/questions/17617420/can-i-avoid-all-this-multiples-try-catch   see the nullptr paradigm with boost::any_cast pointer variant.
// TODO: implement out of class casts and return own exception. Use typeid to check i.s.o. dynamic_cast.

template<class T>
using StorageType = typename std::decay<T>::type;

class Any
{
public:
  bool is_null() const { return !iPtr; }
  bool not_null() const { return iPtr; }

  template<typename U> Any(U&& value)
    : iPtr(new Derived<StorageType<U>>(std::forward<U>(value)))
  {
  }

  template<class U> bool is() const
  {
    typedef StorageType<U> T;

    auto derived = dynamic_cast<Derived<T>*> (iPtr);

    return derived;
  }

  template<class U>
  StorageType<U>& as()
  {
    typedef StorageType<U> T;

    auto derived = dynamic_cast<Derived<T>*> (iPtr);

    if (!derived)
      throw std::bad_cast();

    return derived->value;
  }

  template<class U>
  operator U()
  {
    return as<StorageType<U>>();
  }

  Any()
    : iPtr(nullptr)
  {
  }

  Any(Any& that)
    : iPtr(that.clone())
  {
  }

  Any(Any&& that)
    : iPtr(that.iPtr)
  {
    that.iPtr = nullptr;
  }

  Any(const Any& that)
    : iPtr(that.clone())
  {
  }

  Any(const Any&& that)
    : iPtr(that.clone())
  {
  }

  Any& operator=(const Any& a)
  {
    if (iPtr == a.iPtr)
      return *this;

    auto old_iPtr = iPtr;

    iPtr = a.clone();

    if (old_iPtr)
      delete old_iPtr;

    return *this;
  }

  Any& operator=(Any&& a)
  {
    if (iPtr == a.iPtr)
      return *this;

    std::swap(iPtr, a.iPtr);

    return *this;
  }

  ~Any()
  {
    if (iPtr)
      delete iPtr;
  }

private:
  struct Base
  {
    virtual ~Base() {}
    virtual Base* clone() const = 0;
  };

  template<typename T>
  struct Derived : Base
  {
    template<typename U> Derived(U&& value) : value(std::forward<U>(value)) { }

    T value;

    Base* clone() const { return new Derived<T>(value); }
  };

  Base* clone() const
  {
    if (iPtr)
      return iPtr->clone();
    else
      return nullptr;
  }

private:
  Base* iPtr;
};

#endif // ANY_H
