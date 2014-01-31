#include "node_collection.h"

#include <nodes/models/connection.h>
#include <nodes/models/field.h>
#include <nodes/models/node.h>

#include <nodes/nodes/base.h>
#include <nodes/nodes/math.h>

std::shared_ptr<Connection> NodeCollection::createConnection(Field& aFromField, Field& aToField)
{
  iConnections.emplace_back(std::make_shared<Connection>(aFromField, aToField));
  return iConnections.back();
}

std::shared_ptr<Node> NodeCollection::createNode(const std::string& aNodeType)
{
  if (aNodeType == "Number")
  {
    iNodes.emplace_back(std::make_shared<Number>());
  }
  else if (aNodeType == "Add")
  {
    iNodes.emplace_back(std::make_shared<Add>());
  }

  return iNodes.back();
}

void NodeCollection::compute()
{
  for (auto node : iNodes)
  {
    evaluateSubGraph(node.get());
  }
}

void NodeCollection::evaluateSubGraph(Node* aNode)
{
  for (auto node : aNode->getUpstreamNodes())
  {
    evaluateSubGraph(node);
  }

  if (aNode->isDirty() || aNode->getAutoEvaluate())
  {
    aNode->compute();
    aNode->setDirty(false);
  }
}
