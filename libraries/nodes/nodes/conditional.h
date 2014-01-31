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
    fields().addField("true_val", Field::Direction::Input);
    fields().addField("false_val", Field::Direction::Input);
    fields().addField("out", Field::Direction::Output);

    iCondition = &fields().getField("condition", Field::Direction::Input);
    iTrueVal = &fields().getField("true_val", Field::Direction::Input);
    iFalseVal = &fields().getField("false_val", Field::Direction::Input);
    iOut = &fields().getField("out", Field::Direction::Output);
  }

  void compute()
  {
    bool cond = condition().getValue().as<bool>();

    if (cond)
    {
      out().setValue(trueVal().getValue());
    }
    else
    {
      out().setValue(falseVal().getValue());
    }
  }

  Field& condition()
  {
    assert(iCondition != nullptr);
    return *iCondition;
  }

  Field& trueVal()
  {
    assert(iTrueVal != nullptr);
    return *iTrueVal;
  }

  Field& falseVal()
  {
    assert(iFalseVal != nullptr);
    return *iFalseVal;
  }

  Field& out()
  {
    assert(iOut != nullptr);
    return *iOut;
  }

private:
  Field* iCondition = nullptr;
  Field* iTrueVal = nullptr;
  Field* iFalseVal = nullptr;
  Field* iOut = nullptr;
};

#endif // NODES_CONDITIONAL_H
