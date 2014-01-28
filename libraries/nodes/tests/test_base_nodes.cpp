#include "yaffut.h"

#include <nodes/nodes/base.h>

FUNC(should_create_number_node)
{
  Number n;
  EQUAL(0.0f, n.out().getValue().as<float>());

  n.in().setValue(10.0f);
  n.compute();

  EQUAL(10.0f, n.out().getValue().as<float>());
}

FUNC(should_create_boolean_node)
{
  Boolean b;
  EQUAL(true, b.out().getValue().as<bool>());

  b.boolean().setValue(false);
  b.compute();

  EQUAL(false, b.out().getValue().as<bool>());
}
