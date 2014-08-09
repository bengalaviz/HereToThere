#include <pebble.h>
#include "locations.h"

#define NUM_MENU_ITEMS 1
#define NUM_MENU_SECTIONS 1

static Window *window;
static SimpleMenuLayer *main_menu_layer;
static SimpleMenuSection menu_sections[NUM_MENU_SECTIONS];
static SimpleMenuItem main_menu_items[NUM_MENU_ITEMS];

static void menu_select_callback(int index, void *ctx){
	switch(index){
		case 0:
			locations_show(0);
			break;
	}
}

void window_load(Window *window){
	int num_items=0;
	Layer *window_layer = window_get_root_layer(window);
	
	main_menu_items[num_items++] = (SimpleMenuItem){
		.title = "Locations",
		.subtitle = "My Locations",
		.callback = menu_select_callback
	};
	
	menu_sections[0] = (SimpleMenuSection){
		.num_items = NUM_MENU_ITEMS,
		.items = main_menu_items
	};
	
	main_menu_layer = simple_menu_layer_create(layer_get_bounds(window_layer), window, menu_sections, NUM_MENU_SECTIONS, NULL);
	layer_add_child(window_layer, simple_menu_layer_get_layer(main_menu_layer));
}

void window_unload(Window *window){
	simple_menu_layer_destroy(main_menu_layer);
}

void menu_init(void){
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers){
		.load = window_load,
		.unload = window_unload
	});
	window_stack_push(window, true);
}

void menu_deinit(void){
	window_destroy(window);
}