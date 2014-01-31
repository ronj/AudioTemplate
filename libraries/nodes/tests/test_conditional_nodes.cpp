#include "yaffut.h"

#include <nodes/nodes/conditional.h>

FUNC(should_return_value_based_on_condtion)
{
  IfElse cond;

  cond.trueVal().setValue(10);
  cond.falseVal().setValue(100);
  cond.compute();

  // Alias the out field so only one dynamic_cast is needed.
  int& out = cond.out().getValue().as<int>();

  EQUAL(100, out);

  cond.condition().setValue(true);
  cond.compute();

  EQUAL(10, out);

  cond.condition().setValue(false);
  cond.compute();

  EQUAL(100, out);
}
