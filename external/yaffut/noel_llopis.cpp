#include "yaffut.h"

class MyClass
{
public:
  MyClass(const char*){}
  void doSomethng(){}
  void doSomethingElse(){}
};

/* from: http://www.gamesfromwithin.com/articles/0412/000061.html

SETUP(FixtureTestSuite)
{
  float someNum = 2.0f;
  std::string str = "Hello";
  MyClass someObject("somename");
  someObject.doSomethng();
}
TEARDOWN(FixtureTestSuite)
{
  someObject.doSomethingElse();
}
TEST(FixtureTestSuite, Test1)
{
  ASSERT_CLOSE (someNum, 2.0f);
  someNum = 0.0f;
}
TEST(FixtureTestSuite, Test2)
{
  ASSERT_CLOSE (someNum, 2.0f);
}
TEST(FixtureTestSuite, Test3)
{
  ASSERT_EQUAL(str, "Hello");
}
*/

struct FixtureTestSuite
{
  float someNum;
  std::string str;
  MyClass someObject;

  FixtureTestSuite()
  : someNum(2.0f)
  , str("Hello")
  , someObject("somename") 
  {
    someObject.doSomethng();
  }
  ~FixtureTestSuite()
  {
    someObject.doSomethingElse();
  }
};
TEST(FixtureTestSuite, Test1)
{
  EQUAL(someNum, 2.0f);
}
TEST(FixtureTestSuite, Test2)
{
  someNum = 0.0f;
  EQUAL(someNum, 2.0f);
}
TEST(FixtureTestSuite, Test3)
{
  EQUAL(str, "Hello");
}

