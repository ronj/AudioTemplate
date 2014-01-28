#ifndef NODES_CONDITIONAL_H
#define NODES_CONDITIONAL_H

#include <nodes/models/node.h>

#include <cassert>

class IfElse : public Node
{
public:
  IfElse()
    : Node("IfElse", "Conditional")
  {
    fields().addField("condition", Field::Direction::Input, false);
    fields().addField("val1", Field::Direction::Input);
    fields().addField("val2", Field::Direction::Input);
    fields().addField("out", Field::Direction::Output);

    iCondition = &fields().getField("condition", Field::Direction::Input);
    iVal1 = &fields().getField("val1", Field::Direction::Input);
    iVal2 = &fields().getField("val2", Field::Direction::Input);
    iOut = &fields().getField("out", Field::Direction::Output);
  }

  void compute()
  {
    bool cond = condition().getValue().as<bool>();
    if (cond)
    {
      out().setValue(val1().getValue());
    }
    else
    {
      out().setValue(val2().getValue());
    }
  }

  Field& condition()
  {
    assert(iCondition != nullptr);
    return *iCondition;
  }

  Field& val1()
  {
    assert(iVal1 != nullptr);
    return *iVal1;
  }

  Field& val2()
  {
    assert(iVal2 != nullptr);
    return *iVal2;
  }

  Field& out()
  {
    assert(iOut != nullptr);
    return *iOut;
  }

private:
  Field* iCondition = nullptr;
  Field* iVal1 = nullptr;
  Field* iVal2 = nullptr;
  Field* iOut = nullptr;
};

#endif // NODES_CONDITIONAL_H
