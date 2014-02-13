#include <pebble.h>

static bool __x_collides(GRect first, GRect last) {
  if (first.origin.x >= last.origin.x) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Checking (x): %i, %i", first.origin.x, last.origin.x);
    return last.origin.x + last.size.w > first.origin.x;
  } else {
    APP_LOG(APP_LOG_LEVEL_INFO, "the first shall be last (x): %i, %i", first.origin.x, last.origin.x);
    return __x_collides(last, first);
  }
}

static bool __y_collides(GRect first, GRect last) {
  if (first.origin.y >= last.origin.y) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Checking (y): %i, %i", first.origin.y, last.origin.y);
    return last.origin.y + last.size.h > first.origin.y;
  } else {
    APP_LOG(APP_LOG_LEVEL_INFO, "the first shall be last (y): %i, %i", first.origin.y, last.origin.y);
    return __y_collides(last, first);
  }
}

bool collides(GRect first, GRect last) {
  return __x_collides(first, last) && __y_collides(first, last);
}
