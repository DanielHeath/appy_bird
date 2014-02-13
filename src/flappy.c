#include <pebble.h>
#include "flappy_resource.h"

#define FLAPPY_SIZE_H 16
#define FLAPPY_SIZE_W 10
#define FLAPPY_SIZE { FLAPPY_SIZE_H, FLAPPY_SIZE_W }
// End bitmap resources

typedef struct Flappy {
  PropertyAnimation *property_animation;
  BitmapLayer *bitmap_layer;
} Flappy;

void flappy_create(Flappy* result, Layer *window_layer, GPoint from, GPoint to) {
  BitmapLayer *layer = bitmap_layer_create((GRect) {
    .origin = from,
    .size = FLAPPY_SIZE
  });

  bitmap_layer_set_bitmap(layer, bird_left());
  bitmap_layer_set_compositing_mode(layer, GCompOpClear);
  layer_add_child(window_layer, bitmap_layer_get_layer(layer));

  (*result).bitmap_layer = layer;
  GRect to_rect;
  to_rect = (GRect) {
    .origin = to,
    .size = FLAPPY_SIZE
  };

 (*result).property_animation = property_animation_create_layer_frame(bitmap_layer_get_layer((*result).bitmap_layer), NULL, &to_rect);
  animation_set_duration((Animation*) (*result).property_animation, 2600);
  animation_set_curve((Animation*) (*result).property_animation, AnimationCurveEaseIn);
  animation_schedule((Animation*) (*result).property_animation);
}

void flappy_destroy(Flappy* flappy) {
  property_animation_destroy((*flappy).property_animation);
  bitmap_layer_destroy((*flappy).bitmap_layer);
  free(flappy);
}
