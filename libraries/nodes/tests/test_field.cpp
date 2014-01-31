#include "yaffut.h"
#include "test_node.h"

#include <nodes/models/field.h>

FUNC(should_create_field)
{
  TestNode n;
  Field f(n, "FieldName", Field::Direction::Input);

  CHECK(f.getValue().is_null());
}

FUNC(should_set_default_value)
{
  TestNode n;
  Field f(n, "", Field::Direction::Output);

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
  TestNode n;
  Field f(n, "", Field::Direction::Output);

  f.setValue(1.0f);
  EQUAL(1.0f, f.getValue().as<float>());

  f.setValue(123.456f);
  EQUAL(123.456f, f.getValue().as<float>());
}
