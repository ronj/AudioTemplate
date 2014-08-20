#include "yaffut.h"

struct SpecificSuiteCtors
{
  int i;
  std::string s;

  SpecificSuiteCtors(): i(0), s("default value")
  {
    EQUAL(0, i); EQUAL("default value", s);
  }
  SpecificSuiteCtors(int i_): i(i_), s("default value")
  {
    EQUAL(0, i);
  }
  SpecificSuiteCtors(int i_, const std::string& s_): i(i_), s(s_)
  {
    EQUAL("default value", s);
  }
};

struct CaseA: public yaffut::Test<SpecificSuiteCtors, CaseA>
{
  CaseA(): SpecificSuiteCtors(){}
};

struct CaseB: public yaffut::Test<SpecificSuiteCtors, CaseB>
{
  CaseB(): SpecificSuiteCtors(123){}
};

TEST(SpecificSuiteCtors, CaseC): SpecificSuiteCtors(123, "blah"){}

struct CaseD: public yaffut::Test<SpecificSuiteCtors, CaseD>
{
  CaseD(){}
};

