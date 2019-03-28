#ifndef XKCD_PRIVATE_H
#define XKCD_PRIVATE_H

#include "optional.h"
#include <stdbool.h> // bool

#define COMIC_TEXT_MAX_SIZE 1024
struct comic
{
  char text[COMIC_TEXT_MAX_SIZE];
  unsigned char* data;
  size_t data_size;
};

#define COMIC_ID_MAX_SIZE 64
struct xkcd
{
  bool has_previous;
  char previous_id[COMIC_ID_MAX_SIZE];
  bool has_next;
  char next_id[COMIC_ID_MAX_SIZE];
  struct comic image;
};

enum xkcd_direction
{
  XKCD_DIRECTION_PREVIOUS,
  XKCD_DIRECTION_NEXT
};

struct optional
download_raw_data(char const* const url);

struct optional
download_raw_html(char const* const comic_id);

struct optional
get_xpath_context(struct dynamic_buffer* const raw_html);

struct optional
get_attribute_value(xmlAttrPtr attribute_list,
                    char const* const attribute_name);

bool
get_comic_id(xmlXPathContextPtr xpath,
             enum xkcd_direction const direction,
             bool* has_result,
             char* result);

bool
download_comic(xmlXPathContextPtr xpath, struct comic* result);

#endif /* XKCD_PRIVATE_H */
