#ifndef FIELD_H
#define FIELD_H

#include <nodes/utils/any.h>
#include <nodes/utils/indexer.h>

#include <vector>
#include <string>

class Connection;
class Node;

class Field
{
public:
  enum class Direction { Input, Output };

public:
  Field();
  Field(Node& aNode, const std::string& aName, Direction aDirection, Any aValue = Any());

  Node& getNode() const;

  bool isOutput() const;

  void removeConnections();
  void addConnection(Connection* aConnection);
  void unregisterConnection(Connection* aConnection);
  const std::vector<Connection*>& getConnections() const;
  bool isConnected() const;

  void setChanged(bool aChanged);
  bool isChanged();

  void setValue(Any aValue);
  Any getValue() const;

  void setDefaultValue(Any aDefaultValue);

protected:
  Any onValueChanged(Any aValue)
  {
    return computeValue(aValue);
  }

  Any computeValue(Any aValue)
  {
    return aValue;
  }

private:
  void setNodeAndParentsDirty(Node& aNode);

private:
  Indexer iIndexer;
  unsigned int iFieldID;
  Node* iNode;
  std::string iName;
  bool iIsOutput;
  bool iIsChanged = true;
  std::vector<Connection*> iConnections;
  Any iValue;
  Any iDefaultValue;
};

#endif // FIELD_H
