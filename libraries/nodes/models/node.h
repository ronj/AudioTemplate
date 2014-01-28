#ifndef NODE_H
#define NODE_H

#include <nodes/utils/indexer.h>
#include <nodes/collections/field_collection.h>

#include <algorithm>
#include <string>
#include <vector>

class Connection;
class Field;

class Node
{
public:
  Node(const std::string& aName, const std::string& aGroup, Node* aParent = nullptr);

  unsigned int nodeID() const;
  const std::string& name() const;

  void setDirty();

  void addOutConnection(Connection* aConnection, Field* aField);
  void removeConnection(Connection* aConnection);

  bool hasParent() const;
  Node* getParent() const;

  std::vector<Node*> getUpstreamNodes() const;
  std::vector<Node*> getDownstreamNodes() const;

  FieldCollection& fields();

private:
  Indexer iIndexer;
  unsigned int iNodeID;
  std::string iName;
  std::string iGroup;
  bool iAutoEvaluate;
  bool iDirty;
  Node* iParent;
  std::vector<Connection*> iOutConnections;
  FieldCollection iFields;
};

#endif // NODE_H
