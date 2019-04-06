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
#include "xkcd.h"
#include "dynamic_buffer.h" // dynamic_buffer, dynamic_buffer_*
#include "optional.h"       // optional
#include "xkcd_private.h"
#include <assert.h>            // assert
#include <curl/curl.h>         // CURL, curl_easy_*, CURLcode
#include <libxml/HTMLparser.h> // html*
#include <libxml/xpath.h>      // xml*, XML_ELEMENT_NODE
#include <stddef.h>            // size_t
#include <stdlib.h>            // malloc
#include <string.h>            // memcpy, strlen, strcpy

static size_t
write_callback(char* curl_data, size_t always_1, size_t size, void* user_data)
{
  assert(always_1 == 1);
  size_t result = 0;
  struct dynamic_buffer* buffer = (struct dynamic_buffer*)user_data;

  enum dynamic_buffer_error_code error_code =
    dynamic_buffer_append(buffer, (unsigned char*)curl_data, size);
  if (error_code == DYNAMIC_BUFFER_OK) {
    result = size;
  }

  return result;
}

struct optional
download_raw_data(char const* const url)
{
  struct optional result = optional();

  /* Download the URL's data using libcurl. */
  struct dynamic_buffer raw_data = dynamic_buffer();

  CURL* curl = curl_easy_init();
  if (curl == NULL) {
    goto end;
  }

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &raw_data);

  CURLcode curl_error_code = curl_easy_perform(curl);
  if (curl_error_code == CURLE_OK) {
    result.value_type = OPTIONAL_BUFFER;
    result.u.buffer = raw_data;
  } else {
    dynamic_buffer_cleanup(&raw_data);
  }

  curl_easy_cleanup(curl);

end:
  return result;
}

#define URL_MAX_SIZE 1024
#define XKCD_URL "http://xkcd.com/"
struct optional
download_raw_html(char const* const comic_id)
{
  char url[URL_MAX_SIZE] = XKCD_URL;
  strcat(url, comic_id);

  return download_raw_data(url);
}

struct optional
get_xpath_context(struct dynamic_buffer* const raw_html)
{
  struct optional result = optional();

  htmlParserOption const options = HTML_PARSE_RECOVER | HTML_PARSE_NOERROR |
                                   HTML_PARSE_NOWARNING | HTML_PARSE_NONET |
                                   HTML_PARSE_NOIMPLIED;
  htmlDocPtr html =
    htmlReadMemory((char*)raw_html->data, raw_html->size, NULL, NULL, options);
  if (html == NULL) {
    goto end;
  }

  xmlXPathContextPtr xpath = xmlXPathNewContext(html);
  if (xpath != NULL) {
    result.value_type = OPTIONAL_XPATH;
    result.u.xpath = xpath;
  } else {
    xmlFreeDoc(html);
  }

end:
  return result;
}

struct optional
get_attribute_value(xmlAttrPtr attribute_list, char const* const attribute_name)
{
  struct optional result = optional();

  for (xmlAttrPtr attribute_object = attribute_list; attribute_object != NULL;
       attribute_object = attribute_object->next) {
    if (strcmp(attribute_name, (char const*)attribute_object->name) == 0) {
      result.value_type = OPTIONAL_STRING;
      result.u.string = (char const*)attribute_object->children->content;
      goto end;
    }
  }

end:
  return result;
}

#define XPATH_DIRECTION(direction)                                             \
  "//ul[@class=\"comicNav\"]/li/a[@rel=\"" #direction "\"]"
#define ATTRIBUTE_HREF "href"
#define SLASH "/"
#define HASH "#"
bool
get_comic_id(xmlXPathContextPtr xpath,
             enum xkcd_direction const direction,
             bool* has_result,
             char* result)
{
  bool error = true;

  /*
    Extract the previous/next comic's ID from the HTML using libxml2.

    Sample HTML:
    <ul class="comicNav">
      ...
      <li><a rel="prev" href="/302/" accesskey="p">&lt; Prev</a></li>
      ...
      <li><a rel="next" href="/304/" accesskey="p">Next &gt;</a></li>
      ...
    </ul>
  */
  char const* xpath_expression = NULL;
  switch (direction) {
    case XKCD_DIRECTION_PREVIOUS:
      xpath_expression = XPATH_DIRECTION(prev);
      break;
    case XKCD_DIRECTION_NEXT:
      xpath_expression = XPATH_DIRECTION(next);
      break;
  }

  xmlXPathObjectPtr xpath_object =
    xmlXPathEval((xmlChar const*)xpath_expression, xpath);
  if (xpath_object == NULL) {
    goto end;
  }
  if (xpath_object->nodesetval == NULL) {
    goto end_with_cleanup;
  }
  if (xpath_object->nodesetval->nodeNr == 0) {
    goto end_with_cleanup;
  }

  xmlNodePtr a_object = xpath_object->nodesetval->nodeTab[0];
  if (a_object->type != XML_ELEMENT_NODE) {
    goto end_with_cleanup;
  }

  // <a rel="next" href="/304/" accesskey="p">Next &gt;</a>
  // <a rel="prev" href="/302/" accesskey="p">&lt; Prev</a>
  struct optional maybe_a_href =
    get_attribute_value(a_object->properties, ATTRIBUTE_HREF);
  if (maybe_a_href.value_type != OPTIONAL_STRING) {
    goto end_with_cleanup;
  }
  char const* const a_href = maybe_a_href.u.string;

  char href[COMIC_ID_MAX_SIZE];
  strcpy(href, a_href);
  char* token = strtok(href, SLASH);
  if (token == NULL) {
    goto end_with_cleanup;
  }

  error = false;

  if (strcmp(HASH, token) == 0) {
    *has_result = false;
  } else {
    *has_result = true;
    strcpy(result, token);
  }

end_with_cleanup:
  /* Cleanup resources. */
  xmlXPathFreeObject(xpath_object);
end:
  return error;
}

#define XPATH_COMIC_IMG "//div[@id=\"comic\"]//img"
#define DOUBLE_SLASH "//"
#define DOUBLE_SLASH_SIZE 2
#define ATTRIBUTE_SRC "src"
#define ATTRIBUTE_TITLE "title"
#define HTTPS_COLON "https:"
bool
download_comic(xmlXPathContextPtr xpath, struct comic* result)
{
  bool error = true;

  /*
    Extract the comic's URL from the HTML using libxml2.

    Sample HTML:
    <div id="comic">
      <img src="//imgs.xkcd.com/comics/xxx.png" title="<tooltip text>">
    </div>
  */
  xmlXPathObjectPtr xpath_object =
    xmlXPathEval((xmlChar const*)XPATH_COMIC_IMG, xpath);
  if (xpath_object == NULL) {
    goto end;
  }
  if (xpath_object->nodesetval == NULL) {
    goto end_with_cleanup;
  }
  if (xpath_object->nodesetval->nodeNr != 1) {
    goto end_with_cleanup;
  }

  xmlNodePtr img_object = xpath_object->nodesetval->nodeTab[0];
  if (img_object->type != XML_ELEMENT_NODE) {
    goto end_with_cleanup;
  }

  // <img ... src="//imgs.xkcd.com/comics/xxx.png" ...>
  struct optional maybe_img_src =
    get_attribute_value(img_object->properties, ATTRIBUTE_SRC);
  if (maybe_img_src.value_type != OPTIONAL_STRING) {
    goto end_with_cleanup;
  }
  char const* const img_src = maybe_img_src.u.string;
  if (strncmp(img_src, DOUBLE_SLASH, DOUBLE_SLASH_SIZE) != 0) {
    goto end_with_cleanup;
  }

  /* Download the comic image using libcurl. */
  char image_url[URL_MAX_SIZE] = HTTPS_COLON;
  strcat(image_url, img_src);
  struct optional maybe_raw_image = download_raw_data(image_url);
  if (maybe_raw_image.value_type != OPTIONAL_BUFFER) {
    goto end_with_cleanup;
  }
  free(result->data);
  dynamic_buffer_extract(
    &maybe_raw_image.u.buffer, &result->data, &result->data_size);

  // <img ... title="<tooltip text>" ...>
  struct optional maybe_image_title =
    get_attribute_value(img_object->properties, ATTRIBUTE_TITLE);
  if (maybe_image_title.value_type == OPTIONAL_STRING) {
    strcpy(result->text, maybe_image_title.u.string);
  }

  error = false;

end_with_cleanup:
  /* Cleanup resources. */
  xmlXPathFreeObject(xpath_object);
end:
  return error;
}

struct xkcd*
xkcd_new()
{
  struct xkcd* self = calloc(1, sizeof(struct xkcd));
  return self;
}

void
xkcd_free(struct xkcd* const self)
{
  free(self->image.data);
  free(self);
}

#define LATEST_COMIC_ID ""
enum xkcd_error_code
xkcd_latest(struct xkcd* const self)
{
  return xkcd_jump_to(self, LATEST_COMIC_ID);
}

enum xkcd_error_code
xkcd_jump_to(struct xkcd* const self, char const* const comic_id)
{
  enum xkcd_error_code result = XKCD_ERROR;

  struct optional maybe_raw_html = download_raw_html(comic_id);
  if (maybe_raw_html.value_type != OPTIONAL_BUFFER) {
    goto end;
  }
  struct dynamic_buffer* raw_html = &maybe_raw_html.u.buffer;

  struct optional maybe_xpath = get_xpath_context(raw_html);
  if (maybe_xpath.value_type != OPTIONAL_XPATH) {
    goto end_with_buffer_cleanup;
  }
  xmlXPathContextPtr xpath = maybe_xpath.u.xpath;

  bool error = get_comic_id(
    xpath, XKCD_DIRECTION_PREVIOUS, &self->has_previous, self->previous_id);
  if (error) {
    goto end_with_xml_cleanup;
  }

  error =
    get_comic_id(xpath, XKCD_DIRECTION_NEXT, &self->has_next, self->next_id);
  if (error) {
    goto end_with_xml_cleanup;
  }

  error = download_comic(xpath, &self->image);
  if (!error) {
    result = XKCD_OK;
  }

end_with_xml_cleanup:
  xmlFreeDoc(xpath->doc);
  xmlXPathFreeContext(xpath);
end_with_buffer_cleanup:
  dynamic_buffer_cleanup(raw_html);
end:
  return result;
}

bool
xkcd_has_previous(struct xkcd const* const self)
{
  return self->has_previous;
}

enum xkcd_error_code
xkcd_previous(struct xkcd* const self)
{
  enum xkcd_error_code error_code = XKCD_ERROR;

  if (self->has_previous) {
    error_code = xkcd_jump_to(self, self->previous_id);
  }

  return error_code;
}

bool
xkcd_has_next(struct xkcd const* const self)
{
  return self->has_next;
}

enum xkcd_error_code
xkcd_next(struct xkcd* const self)
{
  enum xkcd_error_code error_code = XKCD_ERROR;

  if (self->has_next) {
    error_code = xkcd_jump_to(self, self->next_id);
  }

  return error_code;
}

enum xkcd_error_code
xkcd_get_comic(struct xkcd const* const self,
               unsigned char** image,
               size_t* image_size)
{
  enum xkcd_error_code result = XKCD_ERROR;

  *image = malloc(self->image.data_size);
  if (*image == NULL) {
    goto end;
  }

  *image_size = self->image.data_size;
  memcpy(*image, self->image.data, self->image.data_size);

  result = XKCD_OK;

end:
  return result;
}

enum xkcd_error_code
xkcd_get_text(struct xkcd const* const self, char** text)
{
  enum xkcd_error_code result = XKCD_ERROR;

  *text = calloc(strlen(self->image.text) + 1, sizeof(char));
  if (*text == NULL) {
    goto end;
  }

  strcpy(*text, self->image.text);

  result = XKCD_OK;

end:
  return result;
}
