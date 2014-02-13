#include <pebble.h>

// Bitmap resources
static GBitmap *__bird_left;

GBitmap * bird_left(void) {
  return __bird_left;
}

void flappy_module_init(void) {
  __bird_left = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FLAPPY_LEFT_BLACK);
}

void flappy_module_deinit(void) {
  gbitmap_destroy(__bird_left);
}
