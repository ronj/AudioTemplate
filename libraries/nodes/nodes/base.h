#ifndef NODES_BASE_H
#define NODES_BASE_H

#include <nodes/nodes/common.h>

struct Number : public NodeNumberUnary
{
  Number() : NodeNumberUnary("Number", "Base") { }
  float process(float aInput) { return aInput; }
};

class Boolean : public Node
{
public:
  Boolean()
    : Node("Boolean", "Base")
  {
    fields().addField("bool", Field::Direction::Input, true);
    fields().addField("out", Field::Direction::Output, true);

    iBool = &fields().getField("bool", Field::Direction::Input);
    iOut = &fields().getField("out", Field::Direction::Output);
  }

  void compute()
  {
    out().setValue(boolean().getValue().as<bool>());
  }

  Field& boolean()
  {
    assert(iBool != nullptr);
    return *iBool;
  }

  Field& out()
  {
    assert(iOut != nullptr);
    return *iOut;
  }

private:
  Field* iBool = nullptr;
  Field* iOut = nullptr;
};

#endif // NODES_BASE_H
