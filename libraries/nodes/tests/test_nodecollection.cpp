#include "yaffut.h"

#include <nodes/collections/node_collection.h>

#include <nodes/nodes/base.h>

FUNC(should_create_node_collection)
{
  NodeCollection nc;

  std::shared_ptr<Node> n1 = nc.createNode("Number");
  std::shared_ptr<Node> n2 = nc.createNode("Number");

  std::shared_ptr<Node> add = nc.createNode("Add");

  nc.createConnection(n1->fields().getField("out", Field::Direction::Output),
                      add->fields().getField("x", Field::Direction::Input));

  nc.createConnection(n2->fields().getField("out", Field::Direction::Output),
                      add->fields().getField("y", Field::Direction::Input));

  n1->fields().getField("in", Field::Direction::Input).setValue(10.0f);
  n2->fields().getField("in", Field::Direction::Input).setValue(5.0f);

  nc.compute();

  EQUAL(15.0f, add->fields().getField("out", Field::Direction::Output).getValue().as<float>());
}
