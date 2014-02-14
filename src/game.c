#include <pebble.h>
#include "game.h"
#include "score.h"
#include "flappy_resource.h"
#include "flappy.h"
#include "collisions.h"

#define SHIP_MOVE_SPEED 3

#define SHIP_SIZE_H 13
#define SHIP_SIZE_W 16
#define SHIP_SIZE { SHIP_SIZE_H, SHIP_SIZE_W }
#define SHIP_OFFSET_FROM_LEFT 8
#define EXIT_STAGE_LEFT -25

static GBitmap *ship;

static struct GameUi {
  Window *window;
  TextLayer *score_text;
  BitmapLayer *ship_bmp;
  Flappy *flappy;
} ui;

static struct GameState {
  bool player_dead;
  unsigned short ship_position;
  unsigned short score;
  AnimationImplementation collider_implementation;
  Animation *collider;
} state;

static void redraw_ship() {
  Layer *internal = bitmap_layer_get_layer(ui.ship_bmp);
  GRect oldbounds = layer_get_frame(internal);
  oldbounds.origin.y = state.ship_position;
  layer_set_frame(internal, oldbounds);
}

static GRect ship_rect() {
  Layer *internal = bitmap_layer_get_layer(ui.ship_bmp);
  return layer_get_frame(internal);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (state.player_dead) {
    return;
  }
  static char buf[32];

  ++state.score;
  snprintf(buf, 32, "Score: %u", state.score);
  text_layer_set_text(ui.score_text, buf);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (state.player_dead) {
    return;
  }
  state.ship_position = state.ship_position - SHIP_MOVE_SPEED;
  redraw_ship();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (state.player_dead) {
    return;
  }
  state.ship_position = state.ship_position + SHIP_MOVE_SPEED;
  redraw_ship();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_UP, 30, up_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 30, down_click_handler);
}

static void cleanup_flappy() {
  if ((int)ui.flappy > 0) {
    flappy_destroy(ui.flappy);
    ui.flappy = 0;
  }
}
static void recreate_flappy() {
  cleanup_flappy();
  Layer *window_layer = window_get_root_layer(ui.window);
  ui.flappy = malloc(sizeof(Flappy));
  flappy_create(ui.flappy, window_layer, (GPoint) { 160, 80 }, (GPoint) { EXIT_STAGE_LEFT, state.ship_position });
}

static void kill_player() {
  state.player_dead = true;

  cleanup_flappy();

  text_layer_set_text(ui.score_text, "You have died.");
}

static void collider_update(struct Animation *animation, const uint32_t time_normalized) {
  if (state.player_dead) {
    return;
  }
  GRect flappy_pos = flappy_bounds(ui.flappy);
  GRect ship_pos = ship_rect();
  if (collides(flappy_pos, ship_pos)) {
    kill_player();
  }
}

static void setup_collision_detection() {
  state.collider = animation_create();
  state.collider_implementation = (AnimationImplementation) {
    .update = collider_update
  };
  animation_set_duration(state.collider, ANIMATION_DURATION_INFINITE);

  animation_set_implementation(state.collider, &state.collider_implementation);
  animation_schedule(state.collider);
}

static void teardown_collision_detection() {
  if ((int)state.collider > 0) {
    animation_destroy(state.collider);
    state.collider = 0;
  }
}

static void reset_game() {
  // When the game window appears, reset the game
  state.score = 0;
  state.player_dead = false;
  state.ship_position = 80;
  text_layer_set_text(ui.score_text, "Select to Start");
  teardown_collision_detection();
  recreate_flappy();
}

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
        .origin = { SHIP_OFFSET_FROM_LEFT, 80 },
        .size = SHIP_SIZE
      });
  bitmap_layer_set_bitmap(ui.ship_bmp, ship);
  bitmap_layer_set_compositing_mode(ui.ship_bmp, GCompOpClear);
  layer_add_child(window_layer, bitmap_layer_get_layer(ui.ship_bmp));

  reset_game();
  redraw_ship();
  setup_collision_detection();
}

static void window_unload(Window *window) {
  text_layer_destroy(ui.score_text);
  bitmap_layer_destroy(ui.ship_bmp);
  cleanup_flappy();
}

void game_init(void) {
  ship = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SHIP_BLACK);
  flappy_module_init();

  ui.window = window_create();

  window_set_click_config_provider(ui.window, click_config_provider);
  window_set_window_handlers(ui.window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(ui.window, animated);
}

void game_deinit(void) {
  gbitmap_destroy(ship);
  flappy_module_deinit();
  window_destroy(ui.window);
}
