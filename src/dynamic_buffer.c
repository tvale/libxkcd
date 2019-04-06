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
#include "dynamic_buffer.h"
#include <assert.h> // assert
#include <stdlib.h> // calloc, malloc, realloc
#include <string.h> // memcpy

struct dynamic_buffer
dynamic_buffer()
{
  struct dynamic_buffer buffer = { .data = NULL, .size = 0 };
  return buffer;
}

struct dynamic_buffer*
dynamic_buffer_new()
{
  struct dynamic_buffer* buffer = calloc(1, sizeof(struct dynamic_buffer));
  return buffer;
}

void
dynamic_buffer_cleanup(struct dynamic_buffer* const buffer)
{
  free(buffer->data);
  buffer->data = NULL;
  buffer->size = 0;
}

void
dynamic_buffer_free(struct dynamic_buffer** const buffer)
{
  dynamic_buffer_cleanup(*buffer);
  free(*buffer);
  *buffer = NULL;
}

enum dynamic_buffer_error_code
dynamic_buffer_append(struct dynamic_buffer* const buffer,
                      unsigned char const* const data,
                      size_t const data_size)
{
  enum dynamic_buffer_error_code result = DYNAMIC_BUFFER_ERROR;

  unsigned char* const new_data =
    realloc(buffer->data, buffer->size + data_size);
  if (new_data == NULL) {
    goto end;
  }
  buffer->data = new_data;

  memcpy(&buffer->data[buffer->size], data, data_size);
  buffer->size += data_size;
  result = DYNAMIC_BUFFER_OK;

end:
  return result;
}

void
dynamic_buffer_extract(struct dynamic_buffer* const buffer,
                       unsigned char** data,
                       size_t* size)
{
  *data = buffer->data;
  *size = buffer->size;

  buffer->data = NULL;
  buffer->size = 0;
}
