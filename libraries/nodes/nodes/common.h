#ifndef NODES_COMMON_H
#define NODES_COMMON_H

#include <nodes/models/node.h>

#include <cassert>

class NodeNumberGenerator : public Node
{
public:
  NodeNumberGenerator(const std::string& aName, const std::string& aGroupName)
    : Node(aName, aGroupName)
    , iOut(fields().addField("out", Field::Direction::Output, 0.0f))
  {
  }

  virtual float process() = 0;

  void compute()
  {
    out().setValue(process());
  }

  Field& out()
  {
    assert(iOut != nullptr);
    return *iOut;
  }

private:
  Field* iOut = nullptr;
};

class NodeNumberUnary : public Node
{
public:
  NodeNumberUnary(const std::string& aName, const std::string& aGroupName)
    : Node(aName, aGroupName)
    , iIn(fields().addField("in", Field::Direction::Input, 0.0f))
    , iOut(fields().addField("out", Field::Direction::Output, 0.0f))
  {
  }

  virtual float process(float aInput) = 0;

  void compute()
  {
    out().setValue(process(in().getValue()));
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

class NodeNumberBinary : public Node
{
public:
  NodeNumberBinary(const std::string& aName, const std::string& aGroupName)
    : Node(aName, aGroupName)
    , iX(fields().addField("x", Field::Direction::Input, 0.0f))
    , iY(fields().addField("y", Field::Direction::Input, 0.0f))
    , iOut(fields().addField("out", Field::Direction::Output, 0.0f))
  {
  }

  virtual float process(float aInput, float aFactor) = 0;

  void compute()
  {
    iOut->setValue(process(iX->getValue(), iY->getValue()));
  }

private:
  Field* iX;
  Field* iY;
  Field* iOut;
};

#endif // NODES_COMMON_H
