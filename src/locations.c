#include <pebble.h>
#include "heretothere.h"
#include "traveltime.h"

static Window *locations_window;
static MenuLayer *location_layer;

struct location_data {
	char *location_name;
	char *type;
	char *trans_mode;
};

struct location_data ld_array[5];

static void get_locations(void){
	APP_LOG(APP_LOG_LEVEL_INFO, "Getting Location");
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
	ld_array[0].location_name = "Not connected to phone";
	layer_mark_dirty(menu_layer_get_layer(location_layer));
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Failed to send AppMessage to Pebble.");
}

static void in_received_handler(DictionaryIterator *iter, void *context){
	Tuple *l1_text = dict_find(iter, L1_TEXT_KEY);
	Tuple *l1_coords = dict_find(iter, L1_COORDS_KEY);
	Tuple *l1_type = dict_find(iter, L1_TYPE_KEY);
	Tuple *l1_trans_mode = dict_find(iter, L1_TRANS_MODE_KEY);
	if (l1_text){
		ld_array[0].location_name = l1_text->value->cstring;
	}
	if (l1_type){
		ld_array[0].type = l1_type->value->cstring;
	}
	if (l1_trans_mode){
		ld_array[0].trans_mode = l1_trans_mode->value->cstring;
	}
	
	Tuple *l2_text = dict_find(iter, L2_TEXT_KEY);
	Tuple *l2_coords = dict_find(iter, L2_COORDS_KEY);
	Tuple *l2_type = dict_find(iter, L2_TYPE_KEY);
	if (l2_text){
		ld_array[1].location_name = l2_text->value->cstring;
	}
	if (l2_type){
		ld_array[1].type = l2_type->value->cstring;
	}
	
	Tuple *l3_text = dict_find(iter, L3_TEXT_KEY);
	Tuple *l3_coords = dict_find(iter, L3_COORDS_KEY);
	Tuple *l3_type = dict_find(iter, L3_TYPE_KEY);
	if (l3_text){
		ld_array[2].location_name = l3_text->value->cstring;
	}
	if (l3_type){
		ld_array[2].type = l3_type->value->cstring;
	}
	
	layer_mark_dirty(menu_layer_get_layer(location_layer));
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
	return 5;
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
			switch (cell_index->row) {
				case 0:
					menu_cell_basic_draw(ctx, cell_layer, ld_array[0].location_name, ld_array[0].type, NULL);
					break;
				case 1:
					menu_cell_basic_draw(ctx, cell_layer, ld_array[1].location_name, ld_array[1].type, NULL);
					break;
				case 2:
					menu_cell_basic_draw(ctx, cell_layer, ld_array[2].location_name, ld_array[2].type, NULL);
					break;
				case 3:
					menu_cell_basic_draw(ctx, cell_layer, ld_array[3].location_name, ld_array[3].type, NULL);
					break;
				case 4:
					menu_cell_basic_draw(ctx, cell_layer, ld_array[4].location_name, ld_array[4].type, NULL);
					break;
			}
			break;
	}
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data){
	switch (cell_index->section){
		case 0:
			switch(cell_index->row){
				case 0:
					traveltime_show("0");
					break;
				case 1:
					traveltime_show("1");
					break;
				case 2:
					traveltime_show("2");
					break;
				case 3:
					traveltime_show("3");
					break;
				case 4:
					traveltime_show("4");
					break;
			}
			break;
	}
}

static void window_load(Window *window){
	int num_items=0;
	Layer *window_layer = window_get_root_layer(locations_window);
	location_layer = menu_layer_create(layer_get_bounds(window_layer));
	
	menu_layer_set_callbacks(location_layer, NULL, (MenuLayerCallbacks){
		.get_header_height = menu_get_header_height_callback,
		.get_num_sections = menu_get_num_sections_callback,
		.get_num_rows = menu_get_num_rows_callback,
		.draw_header = menu_draw_header_callback,
		.draw_row = menu_draw_row_callback,
		.select_click = menu_select_callback
	});
	
	menu_layer_set_click_config_onto_window(location_layer, window);
	layer_add_child(window_layer, menu_layer_get_layer(location_layer));
	
	struct location_data ld1;
	ld1.location_name = "Connecting to phone...";
	ld1.type = "";
	ld_array[0] = ld1;
	
	struct location_data ld2;
	ld2.location_name = "";
	ld2.type="";
	ld_array[1] = ld2;
	
	struct location_data ld3;
	ld3.location_name="";
	ld3.type="";
	ld_array[2] = ld3;
	
	struct location_data ld4;
	ld4.location_name="";
	ld4.type="";
	ld_array[3] = ld4;
	
	struct location_data ld5;
	ld5.location_name="";
	ld5.type="";
	ld_array[4] = ld5;
	
	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler);
	app_message_register_outbox_sent(out_sent_handler);
	app_message_register_outbox_failed(out_failed_handler);
}

static void window_appear(Window *window){
	get_locations();
}

static void window_unload(Window *window){
	menu_layer_destroy(location_layer);
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