#pragma once
#include <pebble.h>

typedef struct Flappy {
  //! The "inherited" state from the "base class", \ref PropertyAnimation.
  PropertyAnimation property_animation;
  BitmapLayer *bitmap_layer;
} Flappy;

GRect flappy_bounds(Flappy* flappy);

void flappy_create(Flappy* result, Layer *window_layer, GPoint from, GPoint to, bool first);
void flappy_destroy(Flappy* flappy);

void flappy_suspend(Flappy* flappy);
void flappy_reanimate(Flappy* flappy);
