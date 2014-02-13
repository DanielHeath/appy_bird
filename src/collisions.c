#include <pebble.h>

static bool __x_collides(GRect first, GRect last) {
  if (first.origin.x >= last.origin.x) {
    return last.origin.x + last.size.w > first.origin.x;
  } else {
    return __x_collides(last, first);
  }
}

static bool __y_collides(GRect first, GRect last) {
  if (first.origin.y >= last.origin.y) {
    return last.origin.y + last.size.h > first.origin.y;
  } else {
    return __y_collides(last, first);
  }
}

bool collides(GRect first, GRect last) {
  return __x_collides(first, last) && __y_collides(first, last);
}
