#include "yaffut.h"

#include <nodes/utils/indexer.h>

FUNC(should_generated_unique_uid)
{
  Indexer indexer;
  unsigned int uidA = indexer.getUID();
  unsigned int uidB = indexer.getUID();

  UNEQUAL(uidA, uidB);
}
