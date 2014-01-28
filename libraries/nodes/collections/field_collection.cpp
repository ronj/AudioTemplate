#include "field_collection.h"

#include <nodes/models/connection.h>
#include <nodes/models/node.h>

FieldCollection::FieldCollection(Node& aNode)
  : iNode(aNode)
{
}

Field& FieldCollection::getField(const field_key& aKey, Field::Direction aDirection)
{
  if (aDirection == Field::Direction::Output)
  {
    return iOutputs[aKey];
  }

  return iInputs[aKey];
}

void FieldCollection::setField(const field_key& aKey, Any aValue)
{
  auto iter = iOutputs.find(aKey);
  if (iter != iOutputs.end())
  {
    iter->second.setValue(aValue);
  }
}

std::vector<Node*> FieldCollection::getUpstreamNodes() const
{
  std::vector<Node*> nodes;
  for (auto input : iInputs)
  {
    for (auto connection : input.second.getConnections())
    {
      nodes.push_back(&connection->fromField().getNode());
    }
  }

  return nodes;
}

std::vector<Node*> FieldCollection::getDownstreamNodes() const
{
  std::vector<Node*> nodes;
  for (auto output : iOutputs)
  {
    for (auto connection : output.second.getConnections())
    {
      nodes.push_back(&connection->toField().getNode());
    }
  }

  return nodes;
}

bool FieldCollection::hasUnconnectedInputs() const
{
  for (auto input : iInputs)
  {
    if (input.second.getConnections().size() == 0)
    {
      return true;
    }
  }

  return false;
}

bool FieldCollection::hasUnconnectedOutputs() const
{
  for (auto output : iOutputs)
  {
    if (output.second.getConnections().size() == 0)
    {
      return true;
    }
  }

  return false;
}

bool FieldCollection::hasUnconnectedFields() const
{
  return hasUnconnectedInputs() || hasUnconnectedOutputs();
}

void FieldCollection::setFieldInputUnchanged()
{
  for (auto input : iInputs)
  {
    input.second.setChanged(false);
  }
}

void FieldCollection::addField(const field_key& aKey, Field::Direction aDirection, Any aValue)
{
  switch(aDirection)
  {
    case Field::Direction::Input:
      iInputs[aKey] = Field(iNode, aKey, aDirection, aValue);
      break;
    case Field::Direction::Output:
      iOutputs[aKey] = Field(iNode, aKey, aDirection, aValue);
      break;
  }
}
