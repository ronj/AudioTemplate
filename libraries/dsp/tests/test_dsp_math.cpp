#include "yaffut.h"

#include <dsp/common/dsp_math.h>

FUNC(should_clamp_values)
{
  EQUAL(clamp(20, 0, 10), 10);
  EQUAL(clamp(0, -10, 10), 0);
  EQUAL(clamp(-20, -10, 0), -10);
}
