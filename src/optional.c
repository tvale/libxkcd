#include "optional.h"

struct optional
optional()
{
  struct optional result = { .value_type = OPTIONAL_NONE };
  return result;
}
