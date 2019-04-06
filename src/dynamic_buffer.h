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
#ifndef DYNAMIC_BUFFER_H
#define DYNAMIC_BUFFER_H
#include <stddef.h> // size_t

struct dynamic_buffer
{
  unsigned char* data;
  size_t size;
};

enum dynamic_buffer_error_code
{
  DYNAMIC_BUFFER_OK = 0,
  DYNAMIC_BUFFER_ERROR
};

struct dynamic_buffer
dynamic_buffer();

struct dynamic_buffer*
dynamic_buffer_new();

void
dynamic_buffer_cleanup(struct dynamic_buffer* const buffer);

void
dynamic_buffer_free(struct dynamic_buffer** const buffer);

enum dynamic_buffer_error_code
dynamic_buffer_append(struct dynamic_buffer* const buffer,
                      unsigned char const* const data,
                      size_t const data_size);

void
dynamic_buffer_extract(struct dynamic_buffer* const buffer,
                       unsigned char** data,
                       size_t* data_size);

#endif /* DYNAMIC_BUFFER_H */
