#include "yaffut.h"

#include <nodes/models/connection.h>
#include <nodes/models/node.h>
#include <nodes/collections/field_collection.h>

FUNC(should_create_fieldcollection)
{
  Node n("", "");
  FieldCollection fc(n);

  // We have not defined inputs or outputs
  // so there should not be unconnected fields.
  CHECK(!fc.hasUnconnectedInputs());
  CHECK(!fc.hasUnconnectedOutputs());
  CHECK(!fc.hasUnconnectedFields());
}

FUNC(should_set_output_field_of_fieldcollection)
{
  Node n("", "");
  FieldCollection fc(n);

  fc.addField("Fs", Field::Direction::Output);
  fc.setField("Fs", 44100.0f);

  EQUAL(44100.0f, fc.getField("Fs", Field::Direction::Output).getValue().as<float>());  
}

FUNC(should_get_upstream_and_downstream_nodes)
{
  Node a("A", "");
  Node b("B", "");
  Node c("C", "");

  FieldCollection fc_a(a);
  FieldCollection fc_b(b);
  FieldCollection fc_c(c);

  fc_a.addField("FieldAInput", Field::Direction::Input);
  fc_a.addField("FieldAOutput", Field::Direction::Output);

  fc_b.addField("FieldBInput", Field::Direction::Input);
  fc_b.addField("FieldBOutput", Field::Direction::Output);

  fc_c.addField("FieldCInput", Field::Direction::Input);
  fc_c.addField("FieldCOutput", Field::Direction::Output);

  CHECK(fc_a.hasUnconnectedInputs());
  CHECK(fc_a.hasUnconnectedOutputs());
  CHECK(fc_a.hasUnconnectedFields());

  Connection conAB(fc_a.getField("FieldAOutput", Field::Direction::Output),
                   fc_b.getField("FieldBInput", Field::Direction::Input));
  Connection conBC(fc_b.getField("FieldBOutput", Field::Direction::Output),
                   fc_c.getField("FieldCInput", Field::Direction::Input));

  CHECK(fc_a.hasUnconnectedInputs());
  CHECK(!fc_a.hasUnconnectedOutputs());
  CHECK(fc_a.hasUnconnectedFields());

  CHECK(!fc_b.hasUnconnectedInputs());
  CHECK(!fc_b.hasUnconnectedOutputs());
  CHECK(!fc_b.hasUnconnectedFields());

  std::vector<Node*> expected = { &a };
  std::vector<Node*> actual = fc_b.getUpstreamNodes();

  CHECK(expected == actual);

  expected.clear();
  actual.clear();

  expected = { &c };
  actual = fc_b.getDownstreamNodes();

  CHECK(expected == actual);
}
