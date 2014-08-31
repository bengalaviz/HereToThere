#include <pebble.h>
#include "heretothere.h"
#include "locations.h"

static Window *traveltime_window;
static GBitmap *res_refresh;
static BitmapLayer *refresh_bitmap_layer;
static GFont s_res_bitham_34_medium_numbers;
static GFont s_res_gothic_14;
static GFont s_res_gothic_18;
static GFont s_res_gothic_14_bold;
static TextLayer *tt_hours_layer;
static TextLayer *tt_hours_label_layer;
static TextLayer *tt_minutes_layer;
static TextLayer *tt_minutes_label_layer;
static Layer *line_layer;
static TextLayer *tt_distance_label_layer;
static TextLayer *tt_distance_layer;
static TextLayer *tt_routing_label_layer;
static TextLayer *tt_dc_layer;
static TextLayer *tt_rt_layer;
static TextLayer *tt_tm_layer;
static TextLayer *tt_tm_label_layer;
static TextLayer *tt_location_layer;
static InverterLayer *tt_inverterlayer;
static int location_num;
static int tuplet_count;
static int refresh_minutes;
static AppTimer *timer;

static void get_time_to_location(void){
	tuplet_count = 0;
	Tuplet requestType = TupletInteger(QUERY_TYPE_KEY, location_num);
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	if (iter == NULL){
		return;
	}
	dict_write_tuplet(iter, &requestType);
	dict_write_end(iter);
	app_message_outbox_send();
}

static void splash_show(){
	res_refresh = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_REFRESH);
	refresh_bitmap_layer = bitmap_layer_create(GRect(0, 0, 144, 152));
	bitmap_layer_set_bitmap(refresh_bitmap_layer, res_refresh);
	layer_add_child(window_get_root_layer(traveltime_window), (Layer *)refresh_bitmap_layer);
}

static void clear_text(){
	text_layer_set_text(tt_location_layer, "");
	text_layer_set_text(tt_hours_layer, "");
	text_layer_set_text(tt_hours_label_layer, "");
	text_layer_set_text(tt_minutes_layer, "");
	text_layer_set_text(tt_minutes_label_layer, "");
	text_layer_set_text(tt_distance_layer, "");
	text_layer_set_text(tt_distance_label_layer, "");
	text_layer_set_text(tt_dc_layer, "");
	text_layer_set_text(tt_rt_layer, "");
	text_layer_set_text(tt_routing_label_layer, "");
	text_layer_set_text(tt_tm_layer, "");
	text_layer_set_text(tt_tm_label_layer, "");
}

static void timer_callback(void *data) {
	if (timer != NULL){
  		app_timer_cancel(timer);
  	}
	vibes_short_pulse();
	clear_text();
	splash_show();
	get_time_to_location();
}

static void splash_hide(){
	bitmap_layer_destroy(refresh_bitmap_layer);
	gbitmap_destroy(res_refresh);
	if (refresh_minutes > 0){
		timer = app_timer_register(refresh_minutes, timer_callback, NULL);
	}
}

void traveltime_out_sent_handler(DictionaryIterator *sent){
}

void traveltime_out_failed_handler(DictionaryIterator *falied, AppMessageResult reason){
	APP_LOG(APP_LOG_LEVEL_INFO, "Failed to send AppMessage to Pebble");
	splash_hide();
	text_layer_set_text(tt_location_layer,"Not connected to phone");
}

void traveltime_in_received_handler(DictionaryIterator *iter){
	Tuple *location_name = dict_find(iter, LOCATION_NAME_KEY);
	Tuple *traffic_time_hours = dict_find(iter, TRAFFIC_TIME_HOURS_KEY);
	Tuple *traffic_time_minutes = dict_find(iter, TRAFFIC_TIME_MINUTES_KEY);
	Tuple *distance = dict_find(iter, LOCATION_DISTANCE_KEY);
	Tuple *distance_conversion = dict_find(iter, DISTANCE_CONVERSION_KEY);
	Tuple *routing_type = dict_find(iter, ROUTING_TYPE_KEY);
	Tuple *transport_mode = dict_find(iter, TRANSPORT_MODE_KEY);
	Tuple *refresh_min = dict_find(iter, REFRESH_MINUTES_KEY);
	
	if (location_name){
		tuplet_count++;
		text_layer_set_text(tt_location_layer, location_name->value->cstring);
	}
	
	if (traffic_time_hours){
		tuplet_count++;
		if (strcmp(traffic_time_hours->value->cstring, "") == 0){
			//Don't print the hrs label
		}else{
			text_layer_set_text(tt_hours_layer, traffic_time_hours->value->cstring);
			text_layer_set_text(tt_hours_label_layer, "hrs");
		}
	}
	
	if (traffic_time_minutes){
		tuplet_count++;
		text_layer_set_text(tt_minutes_layer, traffic_time_minutes->value->cstring);
		text_layer_set_text(tt_minutes_label_layer, "mins");
	}
	
	if (distance){
		tuplet_count++;
		text_layer_set_text(tt_distance_layer, distance->value->cstring);
		text_layer_set_text(tt_distance_label_layer, "Distance :");
	}
	
	if (distance_conversion){
		tuplet_count++;
		text_layer_set_text(tt_dc_layer, distance_conversion->value->cstring);
	}
	
	if (routing_type){
		tuplet_count++;
		text_layer_set_text(tt_rt_layer, routing_type->value->cstring);
		text_layer_set_text(tt_routing_label_layer, "Routing Type :");
	}
	
	if (transport_mode){
		tuplet_count++;
		text_layer_set_text(tt_tm_layer, transport_mode->value->cstring);
		text_layer_set_text(tt_tm_label_layer, "Transport Mode :");
	}
	
	if (refresh_min){
		tuplet_count++;
		refresh_minutes = refresh_min->value->int32;
	}
	
	if (tuplet_count > 7){
		splash_hide();
	}
}

static void select_click_handler(ClickRecognizerRef recognizer, Window *window){
	clear_text();
	splash_show();
	get_time_to_location();
}

static void tt_window_config_provider(void *context){
	window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) select_click_handler);
}

static void update_line_layer_callback(Layer *layer, GContext* ctx){
	graphics_draw_line(ctx, GPoint(0, 22), GPoint(144, 22));
	graphics_draw_line(ctx, GPoint(0, 63), GPoint(144, 63));
	graphics_draw_line(ctx, GPoint(0, 93), GPoint(144, 93));
	graphics_draw_line(ctx, GPoint(0, 122), GPoint(144, 122));
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
  	text_layer_set_text_alignment(tt_hours_layer, GTextAlignmentRight);
  	text_layer_set_font(tt_hours_layer, s_res_bitham_34_medium_numbers);
  	layer_add_child(window_layer, (Layer *)tt_hours_layer);
  
  	// tt_hours_label_layer
  	tt_hours_label_layer = text_layer_create(GRect(60, 20, 29, 20));
  	text_layer_set_background_color(tt_hours_label_layer, GColorClear);
  	text_layer_set_font(tt_hours_label_layer, s_res_gothic_14);
  	layer_add_child(window_layer, (Layer *)tt_hours_label_layer);
  
  	// tt_minutes_layer
  	tt_minutes_layer = text_layer_create(GRect(75, 25, 42, 34));
  	text_layer_set_background_color(tt_minutes_layer, GColorClear);
  	text_layer_set_text_alignment(tt_minutes_layer, GTextAlignmentRight);
  	text_layer_set_font(tt_minutes_layer, s_res_bitham_34_medium_numbers);
  	layer_add_child(window_layer, (Layer *)tt_minutes_layer);
  
  	// tt_minutes_label_layer
  	tt_minutes_label_layer = text_layer_create(GRect(117, 20, 29, 20));
  	text_layer_set_background_color(tt_minutes_label_layer, GColorClear);
  	text_layer_set_font(tt_minutes_label_layer, s_res_gothic_14);
  	layer_add_child(window_layer, (Layer *)tt_minutes_label_layer);
  
  	// tt_distance_label_layer
  	tt_distance_label_layer = text_layer_create(GRect(5, 62, 138, 18));
  	text_layer_set_background_color(tt_distance_label_layer, GColorClear);
  	text_layer_set_font(tt_distance_label_layer, s_res_gothic_14_bold);
  	layer_add_child(window_layer, (Layer *)tt_distance_label_layer);
  
  	// tt_distance_layer
  	tt_distance_layer = text_layer_create(GRect(6, 72, 115, 18));
  	text_layer_set_background_color(tt_distance_layer, GColorClear);
  	text_layer_set_text_alignment(tt_distance_layer, GTextAlignmentRight);
  	text_layer_set_font(tt_distance_layer, s_res_gothic_18);
  	layer_add_child(window_layer, (Layer *)tt_distance_layer);
  
  	// tt_routing_label_layer
  	tt_routing_label_layer = text_layer_create(GRect(5, 91, 138, 20));
  	text_layer_set_background_color(tt_routing_label_layer, GColorClear);
  	text_layer_set_font(tt_routing_label_layer, s_res_gothic_14_bold);
  	layer_add_child(window_layer, (Layer *)tt_routing_label_layer);
  
  	// tt_dc_layer
  	tt_dc_layer = text_layer_create(GRect(121, 72, 19, 18));
  	text_layer_set_background_color(tt_dc_layer, GColorClear);
  	text_layer_set_text_alignment(tt_dc_layer, GTextAlignmentRight);
  	text_layer_set_font(tt_dc_layer, s_res_gothic_18);
  	layer_add_child(window_layer, (Layer *)tt_dc_layer);
  
  	// tt_rt_layer
  	tt_rt_layer = text_layer_create(GRect(5, 101, 135, 20));
  	text_layer_set_background_color(tt_rt_layer, GColorClear);
  	text_layer_set_text_alignment(tt_rt_layer, GTextAlignmentRight);
  	text_layer_set_font(tt_rt_layer, s_res_gothic_18);
  	layer_add_child(window_layer, (Layer *)tt_rt_layer);
  
  	// tt_tm_layer
  	tt_tm_layer = text_layer_create(GRect(5, 130, 135, 20));
  	text_layer_set_background_color(tt_tm_layer, GColorClear);
  	text_layer_set_text_alignment(tt_tm_layer, GTextAlignmentRight);
  	text_layer_set_font(tt_tm_layer, s_res_gothic_18);
  	layer_add_child(window_layer, (Layer *)tt_tm_layer);
  
  	// tt_tm_label_layer
  	tt_tm_label_layer = text_layer_create(GRect(5, 120, 138, 20));
  	text_layer_set_background_color(tt_tm_label_layer, GColorClear);
  	text_layer_set_font(tt_tm_label_layer, s_res_gothic_14_bold);
  	layer_add_child(window_layer, (Layer *)tt_tm_label_layer);
  
  	// tt_location_layer
  	tt_location_layer = text_layer_create(GRect(1, -2, 140, 20));
  	text_layer_set_background_color(tt_location_layer, GColorClear);
  	text_layer_set_text_alignment(tt_location_layer, GTextAlignmentLeft);
  	text_layer_set_font(tt_location_layer, s_res_gothic_18);
  	layer_add_child(window_layer, (Layer *)tt_location_layer);
  	
  	//tt_invert_layer
  	tt_inverterlayer = inverter_layer_create(GRect(0, 1, 144, 20));
  	layer_add_child(window_layer, (Layer *)tt_inverterlayer);
	
	// line_layer
  	line_layer = layer_create(GRect(0, 0, 144, 152));
  	layer_set_update_proc(line_layer, update_line_layer_callback);
  	layer_add_child(window_layer, (Layer *)line_layer);
	
	splash_show();
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
  	inverter_layer_destroy(tt_inverterlayer);
  	layer_destroy(line_layer);
  	if (timer != NULL){
  		app_timer_cancel(timer);
  	}
}

void traveltime_show(int location){
	location_num = location + 1;
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