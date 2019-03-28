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
