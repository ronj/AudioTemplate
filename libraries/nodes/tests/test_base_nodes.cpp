#include "yaffut.h"

#include <nodes/nodes/base.h>

FUNC(should_create_number_node)
{
  Number n;
  float& f = n.out().getValue().as<float>();

  EQUAL(0.0f, f);

  n.in().setValue(10.0f);
  n.compute();

  EQUAL(10.0f, f);
}

FUNC(should_create_boolean_node)
{
  Boolean b;
  bool& out = b.out().getValue().as<bool>();

  EQUAL(true, out);

  b.boolean().setValue(false);
  b.compute();

  EQUAL(false, out);
}
