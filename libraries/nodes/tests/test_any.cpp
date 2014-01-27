#include "yaffut.h"

#include <nodes/utils/any.h>

#include <string>

FUNC(should_test_any)
{
  Any n;
  CHECK(n.is_null());

  std::string s1 = "foo";

  Any a1 = s1;

  CHECK(a1.not_null());
  CHECK(a1.is<std::string>());
  CHECK(!a1.is<int>());

  Any a2(a1);

  CHECK(a2.not_null());
  CHECK(a2.is<std::string>());
  CHECK(!a2.is<int>());

  std::string s2 = a2;

  EQUAL(s1, s2);
}
