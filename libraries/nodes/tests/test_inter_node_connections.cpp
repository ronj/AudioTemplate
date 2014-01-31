#include "yaffut.h"

#include <nodes/nodes/base.h>
#include <nodes/nodes/conditional.h>
#include <nodes/models/connection.h>

FUNC(should_connect)
{
  Boolean b;
  IfElse cond;

  cond.trueVal().setValue(10);
  cond.falseVal().setValue(100);

  Connection c(b.out(), cond.condition());

  b.boolean().setValue(true);
  b.compute();
  cond.compute();

  int& out = cond.out().getValue().as<int>();

  EQUAL(10, out);

  b.boolean().setValue(false);
  b.compute();
  cond.compute();

  EQUAL(100, out);
}
