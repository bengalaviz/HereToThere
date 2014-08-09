#include <pebble.h>
#include "menu.h"
#include "locations.h"

int QUERY_TYPE_KEY = 1;

int LOCATION_TEXT_KEY = 10;
int LOCATION_COORDS_KEY = 11;
int LOCATION_NUM_KEY = 12;

static void init(void){
	menu_init();
	locations_init();
}

static void deinit(void){
	menu_deinit();
	locations_deinit();
}

int main(void){
	init();
	int inboxMaxSize = app_message_inbox_size_maximum();
	int outboxMaxSize = app_message_outbox_size_maximum();
	app_message_open(inboxMaxSize, outboxMaxSize);
	
	app_event_loop();
	deinit();
	return 0;
}