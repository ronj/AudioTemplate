#include "yaffut.h"

#include <nodes/models/connection.h>
#include <nodes/models/field.h>
#include <nodes/models/node.h>

FUNC(should_create_connection)
{
  Node inNode("", "");
  Node outNode("", "");

  Field inField(inNode, "", Field::Direction::Input);
  Field outField(outNode, "", Field::Direction::Output);

  Connection c(inField, outField);
}

FUNC(should_notify_connected_field)
{
  Node inNode("", "");
  Node outNode("", "");

  Field in(inNode, "", Field::Direction::Input);
  Field out(outNode, "", Field::Direction::Output);

  Connection c(in, out);

  out.setValue("Hello, world!");
  EQUAL("Hello, world!", out.getValue().as<const char*>());
  EQUAL("Hello, world!", in.getValue().as<const char*>());
}
