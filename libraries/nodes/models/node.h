#ifndef NODE_H
#define NODE_H

#include <string>
#include <algorithm>

#include <nodes/utils/indexer.h>

class Connection;
class Field;

class Node
{
public:
  Node(const std::string& aName, const std::string& aGroup)
    : iNodeID(iIndexer.getUID())
    , iName(aName)
    , iGroup(aGroup)
    , iAutoEvaluate(false)
    , iDirty(true)
    , iParent(nullptr)
  {
  }

  unsigned int nodeID()
  {
    return iNodeID;
  }

  void setDirty()
  {
    iDirty = true;
  }

  void addOutConnection(Connection* aConnection, Field* aField)
  {
    if (std::find(iOutConnections.begin(), iOutConnections.end(), aConnection) == iOutConnections.end())
    {
      iOutConnections.push_back(aConnection);
    }
  }

  void removeConnection(Connection* aConnection)
  {
    iOutConnections.erase(std::remove(iOutConnections.begin(), iOutConnections.end(), aConnection), iOutConnections.end());
  }

  bool hasParent() const
  {
    return iParent != nullptr;
  }

  Node* getParent()
  {
    return iParent;
  }

private:
  Indexer iIndexer;
  unsigned int iNodeID;
  std::string iName;
  std::string iGroup;
  bool iAutoEvaluate;
  bool iDirty;
  Node* iParent;
  std::vector<Connection*> iOutConnections;
};

#endif // NODE_H
