#include "yaffut.h"

#include <nodes/models/connection.h>

FUNC(should_create_connection)
{
  Node inNode("", "");
  Node outNode("", "");

  Field inField(inNode, "");
  Field outField(outNode, "", true);

  Connection c(inField, outField);
}

FUNC(should_notify_connected_field)
{
  Node inNode("", "");
  Node outNode("", "");

  Field in(inNode, "");
  Field out(outNode, "", true);

  Connection c(in, out);

  out.setValue("Hello, world!");
  EQUAL("Hello, world!", out.getValue().as<const char*>());
  EQUAL("Hello, world!", in.getValue().as<const char*>());
}
