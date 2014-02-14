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

#define SHOW_FLAPPY false
#define HIDE_FLAPPY true

void _set_flappy_hidden(Flappy *flappy, bool hidden) {
//  APP_LOG(APP_LOG_LEVEL_INFO, "set hidden from %i to %i", (int)layer_get_hidden(bitmap_layer_get_layer((*flappy).bitmap_layer)), (int)hidden);
  layer_set_hidden(bitmap_layer_get_layer((*flappy).bitmap_layer), hidden);
}

void _flappy_pick_new_angle(Flappy* flappy) {
  PropertyAnimation* anim = (*flappy).property_animation;
  (*anim).values.from.grect.origin.y = (rand() % 124) + 20;
  (*anim).values.to.grect.origin.y = (rand() % 124) + 20;
}

void _stop_flappy_anim(struct Animation *animation, bool finished, void *context){
  Flappy *flappy = (Flappy*)context;
  if (finished) {
    _flappy_pick_new_angle(flappy);
    animation_schedule((Animation *)(*flappy).property_animation);
  } else {
    _set_flappy_hidden(flappy, HIDE_FLAPPY);
  }
}
AnimationHandlers flappy_handlers = { NULL, _stop_flappy_anim };

void flappy_create(Flappy* result, Layer *window_layer, GPoint from, GPoint to, bool first) {
  BitmapLayer *layer = bitmap_layer_create((GRect) {
    .origin = from,
    .size = FLAPPY_SIZE
  });

  bitmap_layer_set_compositing_mode(layer, GCompOpClear);

  if (first) {
    bitmap_layer_set_bitmap(layer, bird_left());
  } else {
    bitmap_layer_set_bitmap(layer, duck_hunt());
  }

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

  if (first) {
    animation_set_duration(anim, 2600);
  } else {
    animation_set_duration(anim, 3000);
  }
  animation_set_curve(anim, AnimationCurveEaseIn);
  animation_schedule(anim);

  (*result).property_animation = pa;
}

void flappy_destroy(Flappy* flappy) {
  property_animation_destroy((*flappy).property_animation);
  bitmap_layer_destroy((*flappy).bitmap_layer);
  free(flappy);
}

void flappy_suspend(Flappy* flappy) {
  animation_unschedule((Animation*)(*flappy).property_animation);
}

void flappy_reanimate(Flappy* flappy) {
  _flappy_pick_new_angle(flappy);
  _set_flappy_hidden(flappy, SHOW_FLAPPY);
  animation_schedule((Animation*)(*flappy).property_animation);
}
