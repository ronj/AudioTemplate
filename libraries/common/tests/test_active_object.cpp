#include "yaffut.h"

#include <common/active_object.h>

struct ActiveObjectTest {};

TEST(ActiveObjectTest, should_construct_active_object)
{
  auto ao = ActiveObject::create();
}
