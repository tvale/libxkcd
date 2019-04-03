#define CTEST_MAIN
#include "compiling_png.h"
#include <ctest.h>
#include <xkcd.h>
#include <xkcd_private.h>

CTEST(xkcd_private, download_raw_data_invalid)
{
  struct optional maybe_buffer = download_raw_data("");
  ASSERT_EQUAL(OPTIONAL_NONE, maybe_buffer.value_type);
}

CTEST(xkcd_private, download_raw_data)
{
  struct optional maybe_buffer = download_raw_data("http://www.xkcd.com");
  ASSERT_EQUAL(OPTIONAL_BUFFER, maybe_buffer.value_type);
  dynamic_buffer_cleanup(&maybe_buffer.u.buffer);
}

CTEST(xkcd_private, download_raw_html)
{
  struct optional maybe_buffer = download_raw_html("303");
  ASSERT_EQUAL(OPTIONAL_BUFFER, maybe_buffer.value_type);
  dynamic_buffer_cleanup(&maybe_buffer.u.buffer);
}

CTEST(xkcd_private, get_xpath_context)
{
  struct optional maybe_buffer = download_raw_html("");
  ASSERT_EQUAL(OPTIONAL_BUFFER, maybe_buffer.value_type);

  struct optional maybe_xpath = get_xpath_context(&maybe_buffer.u.buffer);
  ASSERT_EQUAL(OPTIONAL_XPATH, maybe_xpath.value_type);

  xmlFreeDoc(maybe_xpath.u.xpath->doc);
  xmlXPathFreeContext(maybe_xpath.u.xpath);
  dynamic_buffer_cleanup(&maybe_buffer.u.buffer);
}

CTEST(xkcd_private, get_attribute_value_invalid)
{
  struct dynamic_buffer buffer = dynamic_buffer();
  char const* const html =
    "<html><head><body attr=\"value\"></body></head></html>";
  dynamic_buffer_append(&buffer, (unsigned char*)html, strlen(html));

  struct optional maybe_xpath = get_xpath_context(&buffer);
  ASSERT_EQUAL(OPTIONAL_XPATH, maybe_xpath.value_type);

  xmlXPathObjectPtr xpath_object =
    xmlXPathEval((xmlChar const*)"//body", maybe_xpath.u.xpath);
  ASSERT_NOT_NULL(xpath_object);
  ASSERT_EQUAL(1, xpath_object->nodesetval->nodeNr);

  xmlNodePtr body_object = xpath_object->nodesetval->nodeTab[0];
  struct optional maybe_body_nope =
    get_attribute_value(body_object->properties, "nope");
  ASSERT_EQUAL(OPTIONAL_NONE, maybe_body_nope.value_type);

  xmlXPathFreeObject(xpath_object);
  xmlFreeDoc(maybe_xpath.u.xpath->doc);
  xmlXPathFreeContext(maybe_xpath.u.xpath);
  dynamic_buffer_cleanup(&buffer);
}

CTEST(xkcd_private, get_attribute_value)
{
  struct dynamic_buffer buffer = dynamic_buffer();
  char const* const html =
    "<html><head><body attr=\"value\"></body></head></html>";
  dynamic_buffer_append(&buffer, (unsigned char*)html, strlen(html));

  struct optional maybe_xpath = get_xpath_context(&buffer);
  ASSERT_EQUAL(OPTIONAL_XPATH, maybe_xpath.value_type);

  xmlXPathObjectPtr xpath_object =
    xmlXPathEval((xmlChar const*)"//body", maybe_xpath.u.xpath);
  ASSERT_NOT_NULL(xpath_object);
  ASSERT_EQUAL(1, xpath_object->nodesetval->nodeNr);

  xmlNodePtr body_object = xpath_object->nodesetval->nodeTab[0];
  struct optional maybe_body_attr =
    get_attribute_value(body_object->properties, "attr");
  ASSERT_EQUAL(OPTIONAL_STRING, maybe_body_attr.value_type);
  ASSERT_STR("value", maybe_body_attr.u.string);

  xmlXPathFreeObject(xpath_object);
  xmlFreeDoc(maybe_xpath.u.xpath->doc);
  xmlXPathFreeContext(maybe_xpath.u.xpath);
  dynamic_buffer_cleanup(&buffer);
}

static char* const NO_COMIC_ID = "";
static char*
get_comic_id_test(char const* const html,
                  enum xkcd_direction const direction,
                  bool const invalid)
{
  struct dynamic_buffer buffer = dynamic_buffer();
  bool has_comic_id = false;
  char comic_id[1024];

  dynamic_buffer_append(&buffer, (unsigned char*)html, strlen(html));

  struct optional maybe_xpath = get_xpath_context(&buffer);
  ASSERT_EQUAL(OPTIONAL_XPATH, maybe_xpath.value_type);

  bool error =
    get_comic_id(maybe_xpath.u.xpath, direction, &has_comic_id, comic_id);
  ASSERT_EQUAL(invalid, error);

  xmlFreeDoc(maybe_xpath.u.xpath->doc);
  xmlXPathFreeContext(maybe_xpath.u.xpath);
  dynamic_buffer_cleanup(&buffer);

  char* result = NULL;
  if (!invalid) {
    if (has_comic_id) {
      result = malloc(strlen(comic_id) + 1);
      strcpy(result, comic_id);
    } else {
      result = NO_COMIC_ID;
    }
  }
  return result;
}

CTEST(xkcd_private, get_comic_id_invalid)
{
  char const* html = "<html><head><body></body></head></html>";
  get_comic_id_test(html, XKCD_DIRECTION_PREVIOUS, true);
  get_comic_id_test(html, XKCD_DIRECTION_NEXT, true);

  html = "<html><head><body><ul class=\"comicNav\"></ul></body></head></html>";
  get_comic_id_test(html, XKCD_DIRECTION_PREVIOUS, true);
  get_comic_id_test(html, XKCD_DIRECTION_NEXT, true);

  html = "<html><head><body><ul "
         "class=\"comicNav\"><li></li></ul></body></head></html>";
  get_comic_id_test(html, XKCD_DIRECTION_PREVIOUS, true);
  get_comic_id_test(html, XKCD_DIRECTION_NEXT, true);

  html = "<html><head><body><ul class=\"comicNav\"><li><a rel=\"prev\"></a><a "
         "rel=\"next\"></a></li></ul></body></head></html>";
  get_comic_id_test(html, XKCD_DIRECTION_PREVIOUS, true);
  get_comic_id_test(html, XKCD_DIRECTION_NEXT, true);

  html = "<html><head><body><ul class=\"comicNav\"><li><a rel=\"prev\" "
         "href=\"\"></a><a rel=\"next\" "
         "href=\"\"></a></li></ul></body></head></html>";
  get_comic_id_test(html, XKCD_DIRECTION_PREVIOUS, true);
  get_comic_id_test(html, XKCD_DIRECTION_NEXT, true);

  html = "<html><head><body><ul class=\"comicNav\"><li><a rel=\"prev\" "
         "href=\"/\"></a><a rel=\"prev\" "
         "next=\"/\"></a></li></ul></body></head></html>";
  get_comic_id_test(html, XKCD_DIRECTION_PREVIOUS, true);
  get_comic_id_test(html, XKCD_DIRECTION_NEXT, true);

  html = "<html><head><body><ul class=\"comicNav\"><li><a rel=\"prev\" "
         "href=\"//\"></a><a rel=\"next\" "
         "href=\"//\"></a></li></ul></body></head></html>";
  get_comic_id_test(html, XKCD_DIRECTION_PREVIOUS, true);
  get_comic_id_test(html, XKCD_DIRECTION_NEXT, true);
}

CTEST(xkcd_private, get_comic_id)
{
  char const* html = "<html><head><body><ul class=\"comicNav\"><li><a "
                     "rel=\"prev\" href=\"/302/\"></a><a rel=\"next\" "
                     "href=\"/304/\"></a></li></ul></body></head></html>";
  char* comic_id = get_comic_id_test(html, XKCD_DIRECTION_PREVIOUS, false);
  ASSERT_STR("302", comic_id);
  free(comic_id);
  comic_id = get_comic_id_test(html, XKCD_DIRECTION_NEXT, false);
  ASSERT_STR("304", comic_id);
  free(comic_id);

  html = "<html><head><body><ul class=\"comicNav\"><li><a rel=\"prev\" "
         "href=\"#\"></a><a rel=\"next\" "
         "href=\"#\"></a></li></ul></body></head></html>";
  comic_id = get_comic_id_test(html, XKCD_DIRECTION_PREVIOUS, false);
  ASSERT_EQUAL((intptr_t)NO_COMIC_ID, (intptr_t)comic_id);
  comic_id = get_comic_id_test(html, XKCD_DIRECTION_NEXT, false);
  ASSERT_EQUAL((intptr_t)NO_COMIC_ID, (intptr_t)comic_id);
}

static struct comic*
download_comic_test(char const* const html, bool const invalid)
{
  struct dynamic_buffer buffer = dynamic_buffer();
  struct comic image = { .data = NULL, .data_size = 0, .text = "" };

  dynamic_buffer_append(&buffer, (unsigned char*)html, strlen(html));

  struct optional maybe_xpath = get_xpath_context(&buffer);
  ASSERT_EQUAL(OPTIONAL_XPATH, maybe_xpath.value_type);

  bool error = download_comic(maybe_xpath.u.xpath, &image);
  ASSERT_EQUAL(invalid, error);

  xmlFreeDoc(maybe_xpath.u.xpath->doc);
  xmlXPathFreeContext(maybe_xpath.u.xpath);
  dynamic_buffer_cleanup(&buffer);

  struct comic* result = NULL;
  if (!invalid) {
    result = calloc(1, sizeof(struct comic));
    *result = image;
  }
  return result;
}

CTEST(xkcd_private, download_comic_invalid)
{
  char const* html = "<html><head><body></body></head></html>";
  download_comic_test(html, true);

  html = "<html><head><body><div id=\"comic\"></div></body></head></html>";
  download_comic_test(html, true);

  html = "<html><head><body><div id=\"comic\"><img></div></body></head></html>";
  download_comic_test(html, true);

  html = "<html><head><body><div id=\"comic\"><img "
         "src=\"\"></div></body></head></html>";
  download_comic_test(html, true);

  html = "<html><head><body><div id=\"comic\"><img "
         "title=\"value\"></div></body></head></html>";
  download_comic_test(html, true);

  html =
    "<html><head><body><div id=\"comic\"><img "
    "src=\"imgs.xkcd.com/comics/compiling.png\"></div></body></head></html>";
  download_comic_test(html, true);

  html = "<html><head><body><div id=\"comic\"><img "
         "src=\"https://imgs.xkcd.com/comics/compiling.png\"></div></body></"
         "head></html>";
  download_comic_test(html, true);

  html = "<html><head><body><div id=\"comic\"><img "
         "src=\"https://imgs.xkcd.com/comics/compiling.png\"></div></body></"
         "head></html>";
  download_comic_test(html, true);
}

#define COMPILING_PNG_TEXT                                                     \
  "'Are you stealing those LCDs?' 'Yeah, but I'm doing it while my code "      \
  "compiles.'"
static void
validate_comic_303_text(char const* const text)
{
  ASSERT_STR(COMPILING_PNG_TEXT, text);
}

static void
validate_comic_303_image(struct comic const* const image)
{
  ASSERT_DATA(
    compiling_png, sizeof(compiling_png), image->data, image->data_size);
}

CTEST(xkcd_private, download_comic)
{
  char const* html =
    "<html><head><body><div id=\"comic\"><img "
    "src=\"//imgs.xkcd.com/comics/compiling.png\"></div></body></head></html>";
  struct comic* image = download_comic_test(html, false);

  validate_comic_303_image(image);

  free(image->data);
  free(image);

  html =
    "<html><head><body><div id=\"comic\"><img "
    "src=\"//imgs.xkcd.com/comics/compiling.png\" title=\"" COMPILING_PNG_TEXT
    "\"></div></body></head></html>";
  image = download_comic_test(html, false);

  validate_comic_303_image(image);
  validate_comic_303_text(image->text);

  free(image->data);
  free(image);
}

static void
validate_comic_303(struct xkcd const* const xkcd)
{
  validate_comic_303_text(xkcd->image.text);
  validate_comic_303_image(&xkcd->image);
}

CTEST(xkcd_private, new)
{
  struct xkcd* comic = xkcd_new();
  ASSERT_NOT_NULL(comic);
  ASSERT_STR("", comic->previous_id);
  ASSERT_STR("", comic->next_id);
  ASSERT_NULL(comic->image.data);
  ASSERT_EQUAL_U(0, comic->image.data_size);
  ASSERT_STR("", comic->image.text);

  xkcd_free(comic);
}

CTEST(xkcd_public, latest)
{
  struct xkcd* comic = xkcd_new();
  ASSERT_NOT_NULL(comic);

  enum xkcd_error_code error_code = xkcd_latest(comic);
  ASSERT_EQUAL(XKCD_OK, error_code);

  xkcd_free(comic);
}

CTEST(xkcd_public, jump_to)
{
  struct xkcd* comic = xkcd_new();
  ASSERT_NOT_NULL(comic);

  enum xkcd_error_code error_code = xkcd_jump_to(comic, "303");
  ASSERT_EQUAL(XKCD_OK, error_code);

  validate_comic_303(comic);

  xkcd_free(comic);
}

CTEST(xkcd_public, previous)
{
  struct xkcd* comic = xkcd_new();
  ASSERT_NOT_NULL(comic);

  enum xkcd_error_code error_code = xkcd_jump_to(comic, "304");
  ASSERT_EQUAL(XKCD_OK, error_code);

  error_code = xkcd_previous(comic);
  ASSERT_EQUAL(XKCD_OK, error_code);

  validate_comic_303(comic);

  xkcd_free(comic);
}

CTEST(xkcd_public, next)
{
  struct xkcd* comic = xkcd_new();
  ASSERT_NOT_NULL(comic);

  enum xkcd_error_code error_code = xkcd_jump_to(comic, "302");
  ASSERT_EQUAL(XKCD_OK, error_code);

  error_code = xkcd_next(comic);
  ASSERT_EQUAL(XKCD_OK, error_code);

  validate_comic_303(comic);

  xkcd_free(comic);
}

CTEST(xkcd_public, get_comic)
{
  struct xkcd* comic = xkcd_new();
  ASSERT_NOT_NULL(comic);

  enum xkcd_error_code error_code = xkcd_jump_to(comic, "303");
  ASSERT_EQUAL(XKCD_OK, error_code);

  struct comic image;
  error_code = xkcd_get_comic(comic, &image.data, &image.data_size);
  ASSERT_EQUAL(XKCD_OK, error_code);

  validate_comic_303_image(&image);

  free(image.data);
  xkcd_free(comic);
}

CTEST(xkcd_public, get_text)
{
  struct xkcd* comic = xkcd_new();
  ASSERT_NOT_NULL(comic);

  enum xkcd_error_code error_code = xkcd_jump_to(comic, "303");
  ASSERT_EQUAL(XKCD_OK, error_code);

  char* text = NULL;
  error_code = xkcd_get_text(comic, &text);
  ASSERT_EQUAL(XKCD_OK, error_code);

  validate_comic_303_text(text);

  free(text);
  xkcd_free(comic);
}

CTEST(issues, comics_with_hyperlinks)
{
  struct xkcd* comic = xkcd_new();
  ASSERT_NOT_NULL(comic);

  enum xkcd_error_code error_code = xkcd_jump_to(comic, "1000");
  ASSERT_EQUAL(XKCD_OK, error_code);
}

int
main(int argc, char const* argv[])
{
  return ctest_main(argc, argv);
}
