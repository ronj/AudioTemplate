#include "node.h"

Node::Node(const std::string& aName, const std::string& aGroup, Node* aParent)
  : iNodeID(iIndexer.getUID())
  , iName(aName)
  , iGroup(aGroup)
  , iAutoEvaluate(false)
  , iDirty(true)
  , iParent(aParent)
  , iFields(*this)
{
}

unsigned int Node::nodeID() const
{
  return iNodeID;
}

const std::string& Node::name() const
{
  return iName;
}

void Node::setDirty()
{
  iDirty = true;
}

void Node::addOutConnection(Connection* aConnection, Field* aField)
{
  if (std::find(iOutConnections.begin(), iOutConnections.end(), aConnection) == iOutConnections.end())
  {
    iOutConnections.push_back(aConnection);
  }
}

void Node::removeConnection(Connection* aConnection)
{
  iOutConnections.erase(std::remove(iOutConnections.begin(), iOutConnections.end(), aConnection), iOutConnections.end());
}

bool Node::hasParent() const
{
  return iParent != nullptr;
}

Node* Node::getParent() const
{
  return iParent;
}

std::vector<Node*> Node::getUpstreamNodes() const
{
  return iFields.getUpstreamNodes();
}

std::vector<Node*> Node::getDownstreamNodes() const
{
  return iFields.getDownstreamNodes();
}

FieldCollection& Node::fields()
{
  return iFields;
}
