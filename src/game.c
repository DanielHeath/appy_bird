#include <pebble.h>
#include "game.h"
#include "score.h"

// Time always in ms
#define MAX_TIME      5000
#define TIME_INTERVAL 10

static GBitmap *ship;

static struct GameUi {
  Window *window;
  TextLayer *score_text;
  TextLayer *time_text;
  BitmapLayer *ship_bmp;
} ui;

static struct GameState {
  unsigned score;
  unsigned time; // Elapsed time in ms
  uint16_t prev_ms;
  AppTimer *timer;
} state;

static void finish(void) {
  high_score_show();
  high_score_add_score(state.score);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  static char buf[32];
  ++state.score;
  snprintf(buf, 32, "Score: %u", state.score);
  text_layer_set_text(ui.score_text, buf);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(ui.window);
  GRect bounds = layer_get_bounds(window_layer);

  ui.score_text = text_layer_create((GRect) {
        .origin = { 0, 72 },
        .size = { bounds.size.w, 20 }
      });
  text_layer_set_text_alignment(ui.score_text, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(ui.score_text));

  ui.time_text = text_layer_create((GRect) {
        .origin = { 0, 0 },
        .size = { bounds.size.w, 64 }
      });
  text_layer_set_text_alignment(ui.time_text, GTextAlignmentCenter);
  text_layer_set_font(ui.time_text,
                      fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(ui.time_text));

  ui.ship_bmp = bitmap_layer_create((GRect) {
        .origin = { 0, 0 },
        .size = { 13, bounds.size.h}
      });

  bitmap_layer_set_bitmap(ui.ship_bmp, ship);
  bitmap_layer_set_compositing_mode(ui.ship_bmp, GCompOpClear);
  layer_add_child(window_layer, bitmap_layer_get_layer(ui.ship_bmp));
}

static void window_appear(Window *window) {
  // When the game window appears, reset the game
  state.score = 0;
  state.time = 0;
  state.timer = NULL;

  text_layer_set_text(ui.score_text, "Press Select to Start");
}

static void window_unload(Window *window) {
  text_layer_destroy(ui.time_text);
  text_layer_destroy(ui.score_text);
  bitmap_layer_destroy(ui.ship_bmp);
}

void game_init(void) {
  ship = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SHIP_BLACK);

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
  window_destroy(ui.window);
}
