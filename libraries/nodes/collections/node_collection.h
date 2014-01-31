#ifndef NODE_COLLECTION_H
#define NODE_COLLECTION_H

#include <memory>
#include <vector>
#include <string>

class Node;
class Field;
class Connection;

class NodeCollection
{
public:
  std::shared_ptr<Connection> createConnection(Field& aFromField, Field& aToField);
  std::shared_ptr<Node> createNode(const std::string& aNodeType);

  void compute();

private:
  void evaluateSubGraph(Node* aNode);

private:
  std::vector<std::shared_ptr<Node>> iNodes;
  std::vector<std::shared_ptr<Connection>> iConnections;
};

#endif // NODE_COLLECTION_H
