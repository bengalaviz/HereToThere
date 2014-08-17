#include <pebble.h>
#include "heretothere.h"
#include "locations.h"

static Window *traveltime_window;
static GFont s_res_bitham_34_medium_numbers;
static GFont s_res_gothic_14;
static GFont s_res_gothic_18;
static GFont s_res_gothic_14_bold;
static TextLayer *tt_hours_layer;
static TextLayer *tt_hours_label_layer;
static TextLayer *tt_minutes_layer;
static TextLayer *tt_minutes_label_layer;
static TextLayer *tt_distance_label_layer;
static TextLayer *tt_distance_layer;
static TextLayer *tt_routing_label_layer;
static TextLayer *tt_dc_layer;
static TextLayer *tt_rt_layer;
static TextLayer *tt_tm_layer;
static TextLayer *tt_tm_label_layer;
static TextLayer *tt_location_layer;
static char *location_num;

struct location_data{
	char *tt_hours;
	char *tt_minutes;
};

struct location_data locat_data;

static void get_time_to_location(void){
	APP_LOG(APP_LOG_LEVEL_DEBUG, "get_time_to_location");
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
	text_layer_set_text(tt_hours_layer, locat_data.tt_hours);
	text_layer_set_text(tt_minutes_layer, locat_data.tt_minutes);
}

void traveltime_out_sent_handler(DictionaryIterator *sent){
	//outgoing message was delivered
	APP_LOG(APP_LOG_LEVEL_DEBUG, "traveltime_out_send_handler");
}

void traveltime_out_failed_handler(DictionaryIterator *falied, AppMessageResult reason){
	APP_LOG(APP_LOG_LEVEL_INFO, "Failed to send AppMessage to Pebble");
}

void traveltime_in_received_handler(DictionaryIterator *iter){
	Tuple *location_name = dict_find(iter, LOCATION_NAME_KEY);
	Tuple *traffic_time_hours = dict_find(iter, TRAFFIC_TIME_HOURS_KEY);
	Tuple *traffic_time_minutes = dict_find(iter, TRAFFIC_TIME_MINUTES_KEY);
	Tuple *distance = dict_find(iter, LOCATION_DISTANCE_KEY);
	Tuple *distance_conversion = dict_find(iter, DISTANCE_CONVERSION_KEY);
	Tuple *routing_type = dict_find(iter, ROUTING_TYPE_KEY);
	Tuple *transport_mode = dict_find(iter, TRANSPORT_MODE_KEY);
	
	if (location_name){
		text_layer_set_text(tt_location_layer, location_name->value->cstring);
	}
	
	if (traffic_time_hours){
		text_layer_set_text(tt_hours_layer, traffic_time_hours->value->cstring);
	}
	
	if (traffic_time_minutes){
		text_layer_set_text(tt_minutes_layer, traffic_time_minutes->value->cstring);
	}
	
	if (distance){
		text_layer_set_text(tt_distance_layer, distance->value->cstring);
	}
	
	if (distance_conversion){
		text_layer_set_text(tt_dc_layer, distance_conversion->value->cstring);
	}
	
	if (routing_type){
		text_layer_set_text(tt_rt_layer, routing_type->value->cstring);
	}
	
	if (transport_mode){
		text_layer_set_text(tt_tm_layer, transport_mode->value->cstring);
	}
	
	//update_screen();
}

static void select_click_handler(ClickRecognizerRef recognizer, Window *window){
	get_time_to_location();
}

static void tt_window_config_provider(void *context){
	window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) select_click_handler);
}

static void window_load(Window *window){
	window_set_click_config_provider(window, (ClickConfigProvider)tt_window_config_provider);
	Layer *window_layer = window_get_root_layer(traveltime_window);
	
	s_res_bitham_34_medium_numbers = fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS);
  	s_res_gothic_14 = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  	s_res_gothic_18 = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  	s_res_gothic_14_bold = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
  
  	// tt_hours_layer
  	tt_hours_layer = text_layer_create(GRect(0, 25, 59, 37));
  	text_layer_set_background_color(tt_hours_layer, GColorClear);
  	//text_layer_set_text(tt_hours_layer, "199");
  	text_layer_set_text_alignment(tt_hours_layer, GTextAlignmentRight);
  	text_layer_set_font(tt_hours_layer, s_res_bitham_34_medium_numbers);
  	layer_add_child(window_layer, (Layer *)tt_hours_layer);
  
  	// tt_hours_label_layer
  	tt_hours_label_layer = text_layer_create(GRect(60, 20, 29, 20));
  	text_layer_set_background_color(tt_hours_label_layer, GColorClear);
  	text_layer_set_text(tt_hours_label_layer, "hrs");
  	text_layer_set_font(tt_hours_label_layer, s_res_gothic_14);
  	layer_add_child(window_layer, (Layer *)tt_hours_label_layer);
  
  	// tt_minutes_layer
  	tt_minutes_layer = text_layer_create(GRect(75, 25, 42, 34));
  	text_layer_set_background_color(tt_minutes_layer, GColorClear);
  	//text_layer_set_text(tt_minutes_layer, "59");
  	text_layer_set_font(tt_minutes_layer, s_res_bitham_34_medium_numbers);
  	layer_add_child(window_layer, (Layer *)tt_minutes_layer);
  
  	// tt_minutes_label_layer
  	tt_minutes_label_layer = text_layer_create(GRect(117, 20, 29, 20));
  	text_layer_set_background_color(tt_minutes_label_layer, GColorClear);
  	text_layer_set_text(tt_minutes_label_layer, "mins");
  	text_layer_set_font(tt_minutes_label_layer, s_res_gothic_14);
  	layer_add_child(window_layer, (Layer *)tt_minutes_label_layer);
  
  	// tt_distance_label_layer
  	tt_distance_label_layer = text_layer_create(GRect(5, 64, 138, 18));
  	text_layer_set_background_color(tt_distance_label_layer, GColorClear);
  	text_layer_set_text(tt_distance_label_layer, "Distance :");
  	text_layer_set_font(tt_distance_label_layer, s_res_gothic_14_bold);
  	layer_add_child(window_layer, (Layer *)tt_distance_label_layer);
  
  	// tt_distance_layer
  	tt_distance_layer = text_layer_create(GRect(6, 75, 115, 18));
  	text_layer_set_background_color(tt_distance_layer, GColorClear);
  	//text_layer_set_text(tt_distance_layer, "99999999999999.99");
  	text_layer_set_text_alignment(tt_distance_layer, GTextAlignmentRight);
  	text_layer_set_font(tt_distance_layer, s_res_gothic_18);
  	layer_add_child(window_layer, (Layer *)tt_distance_layer);
  
  	// tt_routing_label_layer
  	tt_routing_label_layer = text_layer_create(GRect(5, 91, 138, 20));
  	text_layer_set_background_color(tt_routing_label_layer, GColorClear);
  	text_layer_set_text(tt_routing_label_layer, "Routing Type :");
  	text_layer_set_font(tt_routing_label_layer, s_res_gothic_14_bold);
  	layer_add_child(window_layer, (Layer *)tt_routing_label_layer);
  
  	// tt_dc_layer
  	tt_dc_layer = text_layer_create(GRect(121, 76, 19, 18));
  	text_layer_set_background_color(tt_dc_layer, GColorClear);
  	//text_layer_set_text(tt_dc_layer, "km");
  	text_layer_set_text_alignment(tt_dc_layer, GTextAlignmentRight);
  	text_layer_set_font(tt_dc_layer, s_res_gothic_18);
  	layer_add_child(window_layer, (Layer *)tt_dc_layer);
  
  	// tt_rt_layer
  	tt_rt_layer = text_layer_create(GRect(5, 103, 135, 20));
  	text_layer_set_background_color(tt_rt_layer, GColorClear);
  	//text_layer_set_text(tt_rt_layer, "9999999999999999999");
  	text_layer_set_text_alignment(tt_rt_layer, GTextAlignmentRight);
  	text_layer_set_font(tt_rt_layer, s_res_gothic_18);
  	layer_add_child(window_layer, (Layer *)tt_rt_layer);
  
  	// tt_tm_layer
  	tt_tm_layer = text_layer_create(GRect(5, 130, 135, 20));
  	text_layer_set_background_color(tt_tm_layer, GColorClear);
  	//text_layer_set_text(tt_tm_layer, "9999999999999999999");
  	text_layer_set_text_alignment(tt_tm_layer, GTextAlignmentRight);
  	text_layer_set_font(tt_tm_layer, s_res_gothic_18);
  	layer_add_child(window_layer, (Layer *)tt_tm_layer);
  
  	// tt_tm_label_layer
  	tt_tm_label_layer = text_layer_create(GRect(5, 119, 138, 20));
  	text_layer_set_background_color(tt_tm_label_layer, GColorClear);
  	text_layer_set_text(tt_tm_label_layer, "Transport Mode :");
  	text_layer_set_font(tt_tm_label_layer, s_res_gothic_14_bold);
  	layer_add_child(window_layer, (Layer *)tt_tm_label_layer);
  
  	// tt_location_layer
  	tt_location_layer = text_layer_create(GRect(1, 1, 140, 20));
  	text_layer_set_background_color(tt_location_layer, GColorClear);
  	//text_layer_set_text(tt_location_layer, "Home");
  	text_layer_set_text_alignment(tt_location_layer, GTextAlignmentLeft);
  	text_layer_set_font(tt_location_layer, s_res_gothic_18);
  	layer_add_child(window_layer, (Layer *)tt_location_layer);
	
	get_time_to_location();
}

static void window_unload(Window *window){
  	text_layer_destroy(tt_hours_layer);
  	text_layer_destroy(tt_hours_label_layer);
  	text_layer_destroy(tt_minutes_layer);
  	text_layer_destroy(tt_minutes_label_layer);
  	text_layer_destroy(tt_distance_label_layer);
  	text_layer_destroy(tt_distance_layer);
  	text_layer_destroy(tt_routing_label_layer);
  	text_layer_destroy(tt_dc_layer);
  	text_layer_destroy(tt_rt_layer);
  	text_layer_destroy(tt_tm_layer);
  	text_layer_destroy(tt_tm_label_layer);
  	text_layer_destroy(tt_location_layer);
}

void traveltime_show(char *location){
	location_num = location;
	locat_data.tt_hours = "99";
	locat_data.tt_minutes = "59";
	window_stack_push(traveltime_window, true);
}

void traveltime_init(void){
	traveltime_window = window_create();
	
	window_set_window_handlers(traveltime_window, (WindowHandlers){
		.load = window_load,
		.unload = window_unload
	});
}

void traveltime_deinit(void){
	window_destroy(traveltime_window);
}