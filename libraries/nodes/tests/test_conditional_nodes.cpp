#include "yaffut.h"

#include <nodes/nodes/base.h>
#include <nodes/nodes/conditional.h>
#include <nodes/models/connection.h>

FUNC(should_return_value_based_on_condtion)
{
  IfElse cond;
  //EQUAL(cond.val1().getValue(), Any());

  cond.val1().setValue(10);
  cond.val2().setValue(100);
  cond.condition().setValue(true);
  cond.compute();

  EQUAL(10, cond.out().getValue().as<int>());
}

FUNC(should_connect)
{
  Boolean b;
  IfElse cond;

  cond.val1().setValue(10);
  cond.val2().setValue(100);

  Connection(b.out(), cond.condition());

  b.boolean().setValue(true);
  b.compute();
  cond.compute();

  EQUAL(10, cond.out().getValue().as<int>());

  b.boolean().setValue(false);
  b.compute();
  cond.compute();

  EQUAL(100, cond.out().getValue().as<int>());
}
