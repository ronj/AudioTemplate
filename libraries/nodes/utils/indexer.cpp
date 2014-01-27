#include "indexer.h"

unsigned int Indexer::iUID = 0;

unsigned int Indexer::getUID()
{
  return iUID++;
}
