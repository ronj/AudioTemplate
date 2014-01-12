#include "yaffut.h"

#include <stdexcept>
#include <cassert>

struct Fail
{
  void assert_failure ()
  {
    assert (0);
  }
  void segfault ()
  {
    int *i = 0;
    *i = 0;
  }
};

TEST(Fail, _check)
{
  CHECK(0);
}

TEST(Fail, _equal)
{
  EQUAL("Yaffut", "bloated");
}

TEST(Fail, _unequal)
{
  UNEQUAL("Yaffut", "Yaffut");
}

TEST(Fail, _fail)
{
  FAIL("My failure message");
}

TEST(Fail, _assert_throw)
{
  int a;
  ASSERT_THROW(a = 2, std::logic_error);
}

TEST(Fail, _unexpected_exception)
{
  std::string (0);
}

#if 0
TEST(Fail, _unexpected_assertion)
{
  assert_failure ();
}

TEST(Fail, _unexpected_segfault)
{
  segfault ();
}
#endif

