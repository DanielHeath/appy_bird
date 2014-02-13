#include <pebble.h>
#include "game.h"
#include "score.h"

#define SHIP_MOVE_SPEED 3

#define FLAPPY_SIZE_H 16
#define FLAPPY_SIZE_W 10
#define FLAPPY_SIZE { FLAPPY_SIZE_H, FLAPPY_SIZE_W }

#define SHIP_SIZE_H 13
#define SHIP_SIZE_W 16
#define SHIP_SIZE { SHIP_SIZE_H, SHIP_SIZE_W }

static GBitmap *ship;
static GBitmap *bird_left;

static struct GameUi {
  Window *window;
  TextLayer *score_text;
  BitmapLayer *ship_bmp;
  BitmapLayer *flappy_left_bmp;
} ui;

static struct GameState {
  unsigned ship_position;
  unsigned score;
} state;

static void redraw_ship() {
  Layer *internal = bitmap_layer_get_layer(ui.ship_bmp);
  GRect oldbounds = layer_get_bounds(internal);
  oldbounds.origin.y = state.ship_position;
  layer_set_frame(internal, oldbounds);
}

static GRect ship_rect() {
  Layer *internal = bitmap_layer_get_layer(ui.ship_bmp);
  return layer_get_bounds(internal);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  static char buf[32];

  ++state.score;
  snprintf(buf, 32, "Score: %u", state.score);
  text_layer_set_text(ui.score_text, buf);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  state.ship_position = state.ship_position - SHIP_MOVE_SPEED;
  redraw_ship();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  state.ship_position = state.ship_position + SHIP_MOVE_SPEED;
  redraw_ship();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_UP, 30, up_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 30, down_click_handler);
}

struct PropertyAnimation * flappy_animator;
GRect to_rect;

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(ui.window);
  GRect bounds = layer_get_bounds(window_layer);

  ui.score_text = text_layer_create((GRect) {
        .origin = { 0, 0 },
        .size = { bounds.size.w, 20 }
      });
  text_layer_set_text_alignment(ui.score_text, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(ui.score_text));

  ui.ship_bmp = bitmap_layer_create((GRect) {
        .origin = { 2, 80 },
        .size = SHIP_SIZE
      });
  bitmap_layer_set_bitmap(ui.ship_bmp, ship);
  bitmap_layer_set_compositing_mode(ui.ship_bmp, GCompOpClear);
  layer_add_child(window_layer, bitmap_layer_get_layer(ui.ship_bmp));

  ui.flappy_left_bmp = bitmap_layer_create((GRect) {
        .origin = { 120, 80 },
        .size = FLAPPY_SIZE
      });
  bitmap_layer_set_bitmap(ui.flappy_left_bmp, bird_left);
  bitmap_layer_set_compositing_mode(ui.flappy_left_bmp, GCompOpClear);
  layer_add_child(window_layer, bitmap_layer_get_layer(ui.flappy_left_bmp));

  to_rect = ship_rect();
  to_rect.size.h = FLAPPY_SIZE_H ;
  to_rect.size.w = FLAPPY_SIZE_W ;
  flappy_animator = property_animation_create_layer_frame(bitmap_layer_get_layer(ui.flappy_left_bmp), NULL, &to_rect);
  animation_set_duration((Animation*) flappy_animator, 3000);
  animation_schedule((Animation*) flappy_animator);
}

static void window_appear(Window *window) {
  // When the game window appears, reset the game
  state.score = 0;
  state.ship_position = 80;
  text_layer_set_text(ui.score_text, "Select to Start");
}

static void window_unload(Window *window) {
  text_layer_destroy(ui.score_text);
  bitmap_layer_destroy(ui.ship_bmp);
  bitmap_layer_destroy(ui.flappy_left_bmp);
}

void game_init(void) {
  ship = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SHIP_BLACK);
  bird_left = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FLAPPY_LEFT_BLACK);

  ui.window = window_create();

  window_set_click_config_provider(ui.window, click_config_provider);
  window_set_window_handlers(ui.window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
    .appear = window_appear
  });
  const bool animated = true;
  window_stack_push(ui.window, animated);
}

void game_deinit(void) {
  gbitmap_destroy(ship);
  gbitmap_destroy(bird_left);
  window_destroy(ui.window);
}
