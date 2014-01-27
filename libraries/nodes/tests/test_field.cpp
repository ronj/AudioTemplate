#include "yaffut.h"

#include <nodes/models/field.h>
#include <nodes/models/node.h>

FUNC(should_create_field)
{
  Node n("NodeName", "NodeGroup");
  Field f(n, "FieldName");

  CHECK(f.getValue().is_null());
}

FUNC(should_set_default_value)
{
  Node n("", "");
  Field f(n, "");

  f.setDefaultValue("Default");
  f.setValue(Any());
  EQUAL("Default", f.getValue().as<const char*>());

  f.setValue("Not default");
  EQUAL("Not default", f.getValue().as<const char*>());

  f.setValue(Any());
  EQUAL("Default", f.getValue().as<const char*>());
}

FUNC(should_create_float_field)
{
  Node n("", "");
  Field f(n, "");

  f.setValue(1.0f);
  EQUAL(1.0f, f.getValue().as<float>());

  f.setValue(123.456f);
  EQUAL(123.456f, f.getValue().as<float>());
}
