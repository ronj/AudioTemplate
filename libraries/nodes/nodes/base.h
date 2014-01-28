#ifndef NODES_BASE_H
#define NODES_BASE_H

#include <nodes/models/node.h>

#include <cassert>

class NodeNumberSimple : public Node
{
public:
  NodeNumberSimple(const std::string& aName, const std::string& aGroupName)
    : Node(aName, aGroupName)
  {
    fields().addField("in", Field::Direction::Input, 0.0f);
    fields().addField("out", Field::Direction::Output, 0.0f);

    iIn = &fields().getField("in", Field::Direction::Input);
    iOut = &fields().getField("out", Field::Direction::Output);
  }

  float process(float aInput, unsigned int aIndex)
  {
    return aInput;
  }

  void compute()
  {
    out().setValue(process(in().getValue().as<float>(), 0));
  }

  Field& in()
  {
    assert(iIn != nullptr);
    return *iIn;
  }

  Field& out()
  {
    assert(iOut != nullptr);
    return *iOut;
  }

private:
  Field* iIn = nullptr;
  Field* iOut = nullptr;
};

class Number : public NodeNumberSimple
{
public:
  Number()
    : NodeNumberSimple("Number", "Base")
  {
  }
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
