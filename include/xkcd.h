#ifndef XKCD_H
#define XKCD_H

#include <stddef.h> // size_t

struct xkcd;

enum xkcd_error_code
{
  XKCD_OK = 0,
  XKCD_ERROR
};

struct xkcd*
xkcd_new();

void
xkcd_free(struct xkcd* const self);

enum xkcd_error_code
xkcd_latest(struct xkcd* const self);

enum xkcd_error_code
xkcd_jump_to(struct xkcd* const self, char const* const comic_id);

enum xkcd_error_code
xkcd_previous(struct xkcd* const self);

enum xkcd_error_code
xkcd_next(struct xkcd* const self);

enum xkcd_error_code
xkcd_get_comic(struct xkcd const* const self,
               unsigned char** image,
               size_t* image_size);

enum xkcd_error_code
xkcd_get_text(struct xkcd const* const self, char** text);

#endif /* XKCD_H */
