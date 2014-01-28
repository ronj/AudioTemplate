#ifndef FIELD_COLLECTION_H
#define FIELD_COLLECTION_H

#include <nodes/models/field.h>
#include <nodes/utils/any.h>

#include <unordered_map>
#include <vector>
#include <string>

class Node;

class FieldCollection
{
public:
  typedef std::string field_key;
  typedef std::unordered_map<field_key, Field> field_map;

public:
  explicit FieldCollection(Node& aNode);

  Field& getField(const field_key& aKey, Field::Direction aDirection);
  void setField(const field_key& aKey, Any aValue);

  std::vector<Node*> getUpstreamNodes() const;
  std::vector<Node*> getDownstreamNodes() const;

  bool hasUnconnectedInputs() const;
  bool hasUnconnectedOutputs() const;
  bool hasUnconnectedFields() const;

  void setFieldInputUnchanged();

  void addField(const field_key& aKey, Field::Direction aDirection, Any aValue = Any());

private:
  Node&     iNode;
  field_map iInputs;
  field_map iOutputs;
};

#endif // FIELD_COLLECTION_H
