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

GRect flappy_bounds(Flappy* flappy) {
  Layer *internal = bitmap_layer_get_layer((*flappy).bitmap_layer);
  return layer_get_frame(internal);
}

void _stop_flappy_anim(struct Animation *animation, bool finished, void *context){
  Flappy *result = (Flappy*)context;
  if (finished) {
    animation_schedule((Animation *)(*result).property_animation);
  } else {
    bitmap_layer_set_bitmap((*result).bitmap_layer, INVALID_RESOURCE);
  }
}
AnimationHandlers flappy_handlers = { NULL, _stop_flappy_anim };

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

  PropertyAnimation* pa = property_animation_create_layer_frame(bitmap_layer_get_layer((*result).bitmap_layer), NULL, &to_rect);
  Animation* anim = &(*pa).animation;
  animation_set_handlers(anim, flappy_handlers, result);
  animation_set_duration(anim, 2600);
  animation_set_curve(anim, AnimationCurveEaseIn);
  animation_schedule(anim);
  (*result).property_animation = pa;
}

void flappy_destroy(Flappy* flappy) {
  property_animation_destroy((*flappy).property_animation);
  bitmap_layer_destroy((*flappy).bitmap_layer);
  free(flappy);
}
