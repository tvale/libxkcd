/*
  libxkcd
  Copyright (C) 2019, Tiago Vale

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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
