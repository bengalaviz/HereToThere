#include <pebble.h>
#include "appmessage.h"
#include "heretothere.h"
#include "locations.h"
#include "traveltime.h"

static int query_type;

static void in_received_handler(DictionaryIterator *iter, void *context);
static void in_dropped_handler(AppMessageResult reason, void *context);
static void out_sent_handler(DictionaryIterator *sent, void *context);
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context);

void appmessage_init(void) {
	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler);
	app_message_register_outbox_sent(out_sent_handler);
	app_message_register_outbox_failed(out_failed_handler);

	int inboxMaxSize = app_message_inbox_size_maximum();
	int outboxMaxSize = app_message_outbox_size_maximum();
	app_message_open(inboxMaxSize, outboxMaxSize);
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
	if (dict_find(iter, LOCATIONS_KEY)) {
		locations_in_received_handler(iter);
	}else if (dict_find(iter, TRAVELTIME_KEY)) {
		traveltime_in_received_handler(iter);
	}
}

static void in_dropped_handler(AppMessageResult reason, void *context) {
}

static void out_sent_handler(DictionaryIterator *sent, void *context) {
	if (dict_find(sent, LOCATIONS_KEY)) {
		locations_out_sent_handler(sent);
	}else if (dict_find(sent, TRAVELTIME_KEY)) {
		traveltime_out_sent_handler(sent);
	}
}

static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
	if (dict_find(failed, LOCATIONS_KEY)) {
		locations_out_failed_handler(failed, reason);
	}else if (dict_find(failed, TRAVELTIME_KEY)) {
		traveltime_out_failed_handler(failed, reason);
	}else if (dict_find(failed, QUERY_TYPE_KEY)){
		Tuple *queryType = dict_find(failed, QUERY_TYPE_KEY);
		query_type = queryType->value->int32;
		switch(query_type){
			case 0:
				locations_out_failed_handler(failed, reason);
				break;
			default:
				traveltime_out_failed_handler(failed, reason);
				break;
		}
	}
}