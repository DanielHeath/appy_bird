#include <pebble.h>

// Bitmap resources
static GBitmap *__bird_left;
static GBitmap *__duck_hunt;

GBitmap * bird_left(void) {
  return __bird_left;
}

GBitmap * duck_hunt(void) {
  return __duck_hunt;
}

void flappy_module_init(void) {
  __bird_left = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FLAPPY_LEFT_BLACK);
  __duck_hunt = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DUCKHUNT_LEFT_BLACK);
}

void flappy_module_deinit(void) {
  gbitmap_destroy(__bird_left);
  gbitmap_destroy(__duck_hunt);
}
