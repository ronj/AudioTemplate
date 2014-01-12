// For standalone compilation of this file, uncomment the line below
// #define YAFFUT_MAIN

#include "yaffut.h"

FUNC (checks)
{
  EQUAL ("Yaffut", "Yaf" "fut");
  UNEQUAL ("Yaffut", "Your Foot");
  CHECK (true);
  ASSERT_THROW (FAIL ("this should fail"), yaffut::failure);
}

FUNC (func)
{
  EQUAL ("Yaffut", "Yaffut");
}

struct funcBehindTheScenes: public yaffut::Test<funcBehindTheScenes>
{
  funcBehindTheScenes()
  {
    EQUAL ("Yaffut", "Yaffut");
  }
};
