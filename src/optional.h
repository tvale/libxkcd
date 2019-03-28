#ifndef OPTIONAL_H
#define OPTIONAL_H

#include "dynamic_buffer.h"
#include <libxml/xpath.h>

enum optional_state
{
  OPTIONAL_NONE = 0,
  OPTIONAL_BUFFER,
  OPTIONAL_STRING,
  OPTIONAL_NEW_STRING,
  OPTIONAL_XPATH
};

struct optional
{
  enum optional_state value_type;
  union
  {
    struct dynamic_buffer buffer;
    char const* string;
    char* new_string;
    xmlXPathContextPtr xpath;
  } u;
};

struct optional
optional();

#endif /* OPTIONAL_H */
