#ifndef FIELD_COLLECTION_H
#define FIELD_COLLECTION_H

#include <nodes/models/connection.h>
#include <nodes/models/node.h>
#include <nodes/models/field.h>
#include <nodes/utils/any.h>

#include <unordered_map>
#include <vector>
#include <string>

class FieldCollection
{
public:
  typedef std::string field_key;
  typedef std::unordered_map<field_key, Field> field_map;

public:
  Field& getField(const field_key& aKey, bool aIsOut = false)
  {
    if (aIsOut)
    {
      return iOutputs[aKey];
    }

    return iInputs[aKey];
  }

  void setField(const field_key& aKey, Any aValue)
  {
    auto iter = iOutputs.find(aKey);
    if (iter != iOutputs.end())
    {
      iter->second.setValue(aValue);
    }
  }

  std::vector<Node*> getUpstreamNodes()
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

  std::vector<Node*> getDownstreamNodes()
  {
    std::vector<Node*> nodes;
    for (auto output : iOutputs)
    {
      Field& f = iInputs[output.first];
      for (auto connection : f.getConnections())
      {
        nodes.push_back(&connection->toField().getNode());
      }
    }

    return nodes;
  }

  bool hasUnconnectedInputs()
  {
    for (auto input : iInputs)
    {
      if (input.second.getConnections().size() == 0)
      {
        return true;
      }

      return false;
    }
  }

  bool hasUnconnectedOutputs()
  {
    for (auto output : iOutputs)
    {
      if (output.second.getConnections().size() == 0)
      {
        return true;
      }

      return false;
    }
  }

  bool hasUnconnectedFields()
  {
    return hasUnconnectedInputs() || hasUnconnectedOutputs();
  }

private:
  field_map iInputs;
  field_map iOutputs;
};

#endif // FIELD_COLLECTION_H
