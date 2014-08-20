#define YAFFUT_MAIN
#include "yaffut.h"

using namespace yaffut;

struct Example
{
  double d;
  Example(): d(0.123)
  {
    //setup
  }
  ~Example()
  {
    //teardown
  }
  bool Invariant() const
  {
    return true;
  }
};

struct Case1: public Test<Example, Case1>
{
  Case1()
  {
    equal (0.123, d, __AT__);
  }
};

struct Case2: public Test<Example, Case2>
{
  Case2()
  {
    check (Invariant(), __AT__);
  }
};

struct Case3: public Test<Example, Case3>
{
  Case3()
  {
    unequal ("hello", "world", __AT__);
  }
};

struct Case4: public Test<Example, Case4>
{
  Case4()
  {
    yaffut::assert_throw<std::exception>(this, &Case4::failing_member, __AT__);
  }
  void failing_member()
  {
    fail ("hello", __AT__);
  }
};

template <typename T>
struct Functor
{
  T* t;
  void (T::*mf)();
  Functor(T* t_, void (T::*mf_)()): t(t_), mf(mf_){}
  void operator()() const { (t->*mf)(); }
};

template <typename T>
Functor<T> functor(T* t, void (T::*mf)())
{
  return Functor<T>(t, mf);
}

struct Case5: public Test<Example, Case5>
{
  Case5()
  {
    yaffut::assert_throw<std::exception>(functor(this, &Case5::fail), __AT__);
  }
  void fail()
  {
    throw std::logic_error("failure");
  }
};
