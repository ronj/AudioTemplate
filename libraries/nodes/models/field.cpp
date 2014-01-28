#include "field.h"

#include <nodes/models/connection.h>
#include <nodes/models/node.h>

#include <algorithm>
#include <cassert>

Field::Field()
  : iFieldID(0)
  , iNode(nullptr)
  , iName()
  , iIsOutput(false)
{
}

Field::Field(Node& aNode, const std::string& aName, Direction aDirection, Any aValue)
  : iFieldID(iIndexer.getUID())
  , iNode(&aNode)
  , iName(aName)
  , iIsOutput(aDirection == Direction::Output)
  , iValue(std::move(aValue))
{
}

Node& Field::getNode() const
{
  assert(iNode != nullptr);
  return *iNode;
}

bool Field::isOutput() const
{
  return iIsOutput;
}

void Field::removeConnections()
{
  iConnections.clear();
}

void Field::addConnection(Connection* aConnection)
{
  if (std::find(iConnections.begin(), iConnections.end(), aConnection) == iConnections.end())
  {
    iConnections.push_back(aConnection);

    if (isOutput())
    {
      getNode().addOutConnection(aConnection, this);
    }
  }
}

void Field::unregisterConnection(Connection* aConnection)
{
  getNode().removeConnection(aConnection);

  iConnections.erase(std::remove(iConnections.begin(), iConnections.end(), aConnection), iConnections.end());
}

const std::vector<Connection*>& Field::getConnections() const
{
  return iConnections;
}

bool Field::isConnected() const
{
  return iConnections.size() > 0;
}

void Field::setChanged(bool aChanged)
{
  iIsChanged = aChanged;
}

bool Field::isChanged()
{
  bool wasChanged = iIsChanged;
  iIsChanged = false;

  return wasChanged;
}

void Field::setValue(Any aValue)
{
  iIsChanged = true;
  setNodeAndParentsDirty(getNode());

  Any previousValue = iValue;
  Any newValue = onValueChanged(aValue);

  if (newValue.is_null() && iDefaultValue.not_null())
  {
    previousValue = iDefaultValue;
    newValue = previousValue;
  }

  iValue = newValue;

  if (isOutput())
  {
    for (auto connection : iConnections)
    {
      connection->toField().setValue(newValue);
    }
  }
}

Any Field::getValue() const
{
  return iValue;
}

void Field::setNodeAndParentsDirty(Node& aNode)
{
  aNode.setDirty();

  if (aNode.hasParent())
  {
    setNodeAndParentsDirty(*aNode.getParent());
  }
}

void Field::setDefaultValue(Any aDefaultValue)
{
  iDefaultValue = aDefaultValue;
}
