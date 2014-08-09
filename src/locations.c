#include <pebble.h>
#include "heretothere.h"

static Window *locations_window;
static MenuLayer *menu_layer;

struct location_data{
	char *location_text;
	char *location_coords;
};

struct location_data ld_array[5];

static void get_locations(void){
	Tuplet requestType = TupletCString(QUERY_TYPE_KEY, "getLocations");
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	if (iter == NULL){
		return;
	}
	dict_write_tuplet(iter, &requestType);
	dict_write_end(iter);
	app_message_outbox_send();
}

static void out_sent_handler(DictionaryIterator *sent, void *context){
	// outgoind message was delivered
}

static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context){
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Failed to send AppMessage to Pebble.");
}

static void in_received_handler(DictionaryIterator *iter, void *context){
	Tuple *location_text = dict_find(iter, LOCATION_TEXT_KEY);
	Tuple *location_coords = dict_find(iter, LOCATION_COORDS_KEY);
	Tuple *location_num = dict_find(iter, LOCATION_NUM_KEY);
	
	if (location_text){
		ld_array[0].location_text = location_text->value->cstring;
	}
	
	if (location_coords){
		ld_array[0].location_coords = location_coords->value->cstring;
	}
	
	layer_mark_dirty(menu_layer_get_layer(menu_layer));
}

static void in_dropped_handler(AppMessageResult reason, void *context){
	APP_LOG(APP_LOG_LEVEL_DEBUG, "incoming message from Pebble dropped.");
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data){
	return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data){
	return 1;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data){
	return 1;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data){
	switch (section_index){
		case 0:
			menu_cell_basic_header_draw(ctx, cell_layer, "Locations");
			break;
	}
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data){
	switch(cell_index->section){
		case 0:
			menu_cell_basic_draw(ctx, cell_layer, ld_array[0].location_text, NULL, NULL);
			break;
	}
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data){

}

static void window_load(Window *window){
	Layer *window_layer = window_get_root_layer(locations_window);
	menu_layer = menu_layer_create(layer_get_bounds(window_layer));
	
	menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
		.get_header_height = menu_get_header_height_callback,
		.get_num_sections = menu_get_num_sections_callback,
		.get_num_rows = menu_get_num_rows_callback,
		.draw_header = menu_draw_header_callback,
		.draw_row = menu_draw_row_callback,
		.select_click = menu_select_callback,
	});
	
	menu_layer_set_click_config_onto_window(menu_layer, window);
	layer_add_child(window_layer,menu_layer_get_layer(menu_layer));
	
	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler);
	app_message_register_outbox_sent(out_sent_handler);
	app_message_register_outbox_failed(out_failed_handler);
}

static void window_appear(Window *window){

}

static void window_unload(Window *window){

}

void locations_show(void){
	window_stack_push(locations_window, true);
}

void locations_init(void){
	locations_window = window_create();
	window_set_window_handlers(locations_window, (WindowHandlers){
		.load = window_load,
		.unload = window_unload,
		.appear = window_appear
	});
}

void locations_deinit(void){
	window_destroy(locations_window);
}