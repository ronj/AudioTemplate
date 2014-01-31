#ifndef TEST_NODE
#define TEST_NODE

#include <nodes/models/node.h>

class TestNode : public Node
{
public:
  TestNode() : Node("Test", "Test") { }
  void compute() { }
};


#endif // TEST_NODE
