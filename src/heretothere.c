#include <pebble.h>
#include "menu.h"
#include "locations.h"
#include "traveltime.h"

int QUERY_TYPE_KEY = 1;

int L1_TEXT_KEY = 10;
int L1_COORDS_KEY = 11;
int L1_TYPE_KEY = 12;
int L1_TRANS_MODE_KEY = 13;
int L2_TEXT_KEY = 15;
int L2_COORDS_KEY = 16;
int L2_TYPE_KEY = 17;
int L2_TRANS_MODE_KEY = 18;
int L3_TEXT_KEY = 20;
int L3_COORDS_KEY = 21;
int L3_TYPE_KEY = 22;
int L3_TRANS_MODE_KEY = 23;
int L4_TEXT_KEY = 25;
int L4_COORDS_KEY = 26;
int L4_TYPE_KEY = 27;
int L4_TRANS_MODE_KEY = 28;
int L5_TEXT_KEY = 30;
int L5_COORDS_KEY = 31;
int L5_TYPE_KEY = 32;
int L5_TRANS_MODE_KEY = 33;
int LOCATION_NAME_KEY = 49;
int SUMMARY_DISTANCE_KEY = 50;
int SUMMARY_TRAFFIC_TIME_KEY = 51;
int SUMMARY_BASE_TIME_KEY = 52;
int SUMMARY_TRAVEL_TIME_KEY = 53;

static void init(void){
	menu_init();
	locations_init();
	traveltime_init();
}

static void deinit(void){
	menu_deinit();
	locations_deinit();
	traveltime_deinit();
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