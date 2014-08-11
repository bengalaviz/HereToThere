#include <pebble.h>
#include "heretothere.h"

static Window *traveltime_window;
static TextLayer *location_name;
static TextLayer *l_type;
static TextLayer *l_transport_mode;
static TextLayer *l_distance;
static TextLayer *l_traffic_time;
static TextLayer *l_base_time;
static TextLayer *l_travel_time;
static char *location_num;

struct location_data{
	char *locat_name;
};

struct location_data locat_data;

static void get_time_to_location(void){
	Tuplet requestType = TupletCString(QUERY_TYPE_KEY, location_num);
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	if (iter == NULL){
		return;
	}
	dict_write_tuplet(iter, &requestType);
	dict_write_end(iter);
	app_message_outbox_send();
}

static void update_screen(void){
	//text_layer_set_text(location_name, locat_data.locat_name);
}

static void out_sent_handler(DictionaryIterator *sent, void *context){
	//outgoing message was delivered
}

static void out_failed_handler(DictionaryIterator *falied, AppMessageResult reason, void *context){
	//text_layer_set_text(location_name, "Not connected to phone.");
	APP_LOG(APP_LOG_LEVEL_INFO, "Failed to send AppMessage to Pebble");
}

static void in_received_handler(DictionaryIterator *iter, void *context){
	Tuple *location_name = dict_find(iter, LOCATION_NAME_KEY);
	
	if (location_name){
		locat_data.locat_name = location_name->value->cstring;
	}
	
	update_screen();
}

static void in_dropped_handler(AppMessageResult reason, void *context){
	APP_LOG(APP_LOG_LEVEL_DEBUG, "incoming message from Pebble dropped");
}

static void travel_time_window_config_provider(void *context){
	//text_layer_set_text(location_name, "Refreshing...");
	
	if (bluetooth_connection_service_peek()){
		//get_time_to_location();
	}else{
		//text_layer_set_text(location_name, "Not connected to phone.");
	}
}

static void window_load(Window *window){
	window_set_click_config_provider(window, (ClickConfigProvider)travel_time_window_config_provider);
	Layer *window_layer = window_get_root_layer(traveltime_window);
	
	location_name = text_layer_create(GRect(3,60,141,38));
	text_layer_set_font(location_name, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_background_color(location_name, GColorClear);
	text_layer_set_overflow_mode(location_name, GTextOverflowModeTrailingEllipsis);
	layer_add_child(window_layer, text_layer_get_layer(location_name));
	
	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler);
	app_message_register_outbox_sent(out_sent_handler);
	app_message_register_outbox_failed(out_failed_handler);
}

static void window_unload(Window *window){
	//Destroy text Layers
	text_layer_destroy(location_name);
}

static void window_appear(Window *window){
	text_layer_set_text(location_name, "Connecting to phone...");
	
	if (bluetooth_connection_service_peek()){
		get_time_to_location();
	}else{
		text_layer_set_text(location_name, "Not connected to phone.");
	}
}

void traveltime_show(char *location){
	location_num = location;
	
	locat_data.locat_name = "";
	
	window_stack_push(traveltime_window, true);
}

void traveltime_init(void){
	traveltime_window = window_create();
	
	window_set_window_handlers(traveltime_window, (WindowHandlers){
		.load = window_load,
		.unload = window_unload,
		.appear = window_appear
	});
}

void traveltime_deinit(void){
	window_destroy(traveltime_window);
}