#include <pebble.h>

#define KEY_POWER_SET 0
#define KEY_POWER_STATUS 1
#define KEY_STATUS_REQUEST 2

static Window *s_main_window;
static TextLayer *s_output_layer;

static char s_buffer[64];

static void send_onoff_message(int key, int value) {
	DictionaryIterator *iterator;
	app_message_outbox_begin(&iterator);
	dict_write_int(iterator, key, &value, sizeof(int), false);
	app_message_outbox_send();
}
static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
	APP_LOG(APP_LOG_LEVEL_INFO, "Inside UP handler");
	send_onoff_message(KEY_POWER_SET, 1);
}
static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
	APP_LOG(APP_LOG_LEVEL_INFO, "Inside DOWN handler");
	send_onoff_message(KEY_POWER_SET, 0);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  //text_layer_set_text(text_layer, "Select");
	APP_LOG(APP_LOG_LEVEL_INFO, "Inside click handler...");
	send_onoff_message(KEY_STATUS_REQUEST, 1);
	/*DictionaryIterator *iterator;
	app_message_outbox_begin(&iterator);
	int key = KEY_TURN_OFF;
	int value = 42;
	dict_write_int(iterator, key, &value, sizeof(int), false);
	app_message_outbox_send();*/
}
static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}



static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Get the first pair
  Tuple *data = dict_find(iterator, KEY_POWER_STATUS);
  if (data) {
    //snprintf(s_buffer, sizeof(s_buffer), "Received '%s'", data->value->cstring);
    snprintf(s_buffer, sizeof(s_buffer), "Received '%d'", data->value->int8);
    text_layer_set_text(s_output_layer, s_buffer);
	  
	char tmpval[64];
	snprintf(tmpval, sizeof(tmpval), "RCVD: '%s'", data->value->cstring);
	APP_LOG(APP_LOG_LEVEL_INFO, tmpval);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  // Create output TextLayer
  s_output_layer = text_layer_create(GRect(15, 20, window_bounds.size.w - 5, window_bounds.size.h));
  text_layer_set_font(s_output_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text(s_output_layer, "Waiting...");
  text_layer_set_overflow_mode(s_output_layer, GTextOverflowModeWordWrap);
  layer_add_child(window_layer, text_layer_get_layer(s_output_layer));
}

static void main_window_unload(Window *window) {
  // Destroy output TextLayer
  text_layer_destroy(s_output_layer);
}

static void init() {
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  // Open AppMessage with sensible buffer sizes
  app_message_open(128, 64);

  // Create main Window
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_main_window, true);
  window_set_click_config_provider(s_main_window, click_config_provider);
}

static void deinit() {
  // Destroy main Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
