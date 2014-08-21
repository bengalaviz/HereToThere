#include <pebble.h>
#include "appmessage.h"
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
int LOCATION_DISTANCE_KEY = 50;
int DISTANCE_CONVERSION_KEY = 51;
int TRAFFIC_TIME_MINUTES_KEY = 52;
int TRAFFIC_TIME_HOURS_KEY = 53;
int BASE_TIME_MINUTES_KEY = 54;
int BASE_TIME_HOURS_KEY = 55;
int ROUTING_TYPE_KEY = 56;
int TRANSPORT_MODE_KEY = 57;

int LOCATIONS_KEY = 60;
int TRAVELTIME_KEY = 61;
int REFRESH_MINUTES_KEY = 62;

static void init(void){
	appmessage_init();
	locations_init();
	traveltime_init();
}

static void deinit(void){
	locations_deinit();
	traveltime_deinit();
}

int main(void){
	init();
	
	app_event_loop();
	deinit();
	return 0;
}