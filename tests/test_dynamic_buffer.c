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
#define CTEST_MAIN
#include "dynamic_buffer.h" // dynamic_buffer
#include <ctest.h>          // CTEST, ctest_main

void
test_buffer(struct dynamic_buffer* buffer)
{
  dynamic_buffer_append(buffer, (unsigned char*)"first", 5);
  ASSERT_EQUAL_U(buffer->size, 5);
  ASSERT_DATA(buffer->data, 5, (unsigned char*)"first", 5);

  dynamic_buffer_append(buffer, (unsigned char*)"second", 7);
  ASSERT_EQUAL_U(buffer->size, 12);
  ASSERT_DATA(buffer->data, buffer->size, (unsigned char*)"firstsecond", 12);

  unsigned char* data = NULL;
  size_t size = 0;
  dynamic_buffer_extract(buffer, &data, &size);
  ASSERT_EQUAL_U(buffer->size, 0);
  ASSERT_NULL(buffer->data);
  ASSERT_EQUAL_U(size, 12);
  ASSERT_STR((char*)data, "firstsecond");
  free(data);

  dynamic_buffer_append(buffer, (unsigned char*)"test", 5);
  ASSERT_EQUAL_U(buffer->size, 5);
  ASSERT_STR((char*)buffer->data, "test");
}

CTEST(dynamic_buffer, stack)
{
  struct dynamic_buffer buffer = dynamic_buffer();
  ASSERT_NULL(buffer.data);
  ASSERT_EQUAL_U(buffer.size, 0);

  test_buffer(&buffer);

  dynamic_buffer_cleanup(&buffer);
  ASSERT_NULL(buffer.data);
  ASSERT_EQUAL_U(buffer.size, 0);
}

CTEST(dynamic_buffer, heap)
{
  struct dynamic_buffer* buffer = dynamic_buffer_new();
  ASSERT_NULL(buffer->data);
  ASSERT_EQUAL_U(buffer->size, 0);

  test_buffer(buffer);

  dynamic_buffer_free(&buffer);
  ASSERT_NULL(buffer);
}

int
main(int argc, char const* argv[])
{
  return ctest_main(argc, argv);
}
