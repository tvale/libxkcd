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
#ifndef XKCD_H
#define XKCD_H

#include <stdbool.h>
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

bool
xkcd_has_previous(struct xkcd const* const self);

enum xkcd_error_code
xkcd_previous(struct xkcd* const self);

bool
xkcd_has_next(struct xkcd const* const self);

enum xkcd_error_code
xkcd_next(struct xkcd* const self);

enum xkcd_error_code
xkcd_get_comic(struct xkcd const* const self,
               unsigned char** image,
               size_t* image_size);

enum xkcd_error_code
xkcd_get_text(struct xkcd const* const self, char** text);

#endif /* XKCD_H */
