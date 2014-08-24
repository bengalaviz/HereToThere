#include <pebble.h>
#include "heretothere.h"
#include "traveltime.h"

static Window *locations_window;
static MenuLayer *location_layer;

struct location_data {
	char *location_name;
};

struct location_data ld_array[5];

static void get_locations(void){
	ld_array[0].location_name = "Getting locations";
	layer_mark_dirty(menu_layer_get_layer(location_layer));
	Tuplet requestType = TupletInteger(QUERY_TYPE_KEY, 0);
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	if (iter == NULL){
		return;
	}
	dict_write_tuplet(iter, &requestType);
	dict_write_end(iter);
	app_message_outbox_send();
}

void locations_out_sent_handler(DictionaryIterator *sent){
}

void locations_out_failed_handler(DictionaryIterator *failed, AppMessageResult reason){
	ld_array[0].location_name = "Not connected";
	layer_mark_dirty(menu_layer_get_layer(location_layer));
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Failed to send AppMessage to Pebble.");
}

void locations_in_received_handler(DictionaryIterator *iter){
	Tuple *l1_text = dict_find(iter, L1_TEXT_KEY);
	if (l1_text){
		ld_array[0].location_name = l1_text->value->cstring;
	}
	
	Tuple *l2_text = dict_find(iter, L2_TEXT_KEY);
	if (l2_text){
		ld_array[1].location_name = l2_text->value->cstring;
	}
	
	Tuple *l3_text = dict_find(iter, L3_TEXT_KEY);
	if (l3_text){
		ld_array[2].location_name = l3_text->value->cstring;
	}
	
	Tuple *l4_text = dict_find(iter, L4_TEXT_KEY);
	if (l4_text){
		ld_array[3].location_name = l4_text->value->cstring;
	}
	
	Tuple *l5_text = dict_find(iter, L5_TEXT_KEY);
	if (l5_text){
		ld_array[4].location_name = l5_text->value->cstring;
	}
	
	layer_mark_dirty(menu_layer_get_layer(location_layer));
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data){
	return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data){
	return 1;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data){
	//Need to make this dynamic
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
			menu_cell_basic_draw(ctx, cell_layer, ld_array[cell_index->row].location_name, NULL, NULL);
			break;
	}
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data){
	switch (cell_index->section){
		case 0:
			if (strcmp(ld_array[cell_index->row].location_name, "") == 0){
			
			}else{
				traveltime_show(cell_index->row);
			}
			break;
	}
}

static void window_load(Window *window){
	Layer *window_layer = window_get_root_layer(locations_window);
	
	int num_items=0;
	struct location_data ld1;
	ld1.location_name = "Connecting to phone...";
	ld_array[num_items++] = ld1;
	
	struct location_data ld2;
	ld2.location_name = "";
	ld_array[num_items++] = ld2;
	ld_array[num_items++] = ld2;
	ld_array[num_items++] = ld2;
	ld_array[num_items++] = ld2;
	
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
}

static void window_appear(Window *window){
	ld_array[0].location_name = "";
	ld_array[1].location_name = "";
	ld_array[2].location_name = "";
	ld_array[3].location_name = "";
	ld_array[4].location_name = "";
	get_locations();
}

static void window_unload(Window *window){
	menu_layer_destroy(location_layer);
}

void locations_init(void){
	locations_window = window_create();
	
	window_set_window_handlers(locations_window, (WindowHandlers){
		.load = window_load,
		.unload = window_unload,
		.appear = window_appear
	});
	
	window_stack_push(locations_window, true);
}

void locations_deinit(void){
	window_destroy(locations_window);
}