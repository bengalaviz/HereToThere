#include <pebble.h>
#include "appmessage.h"
#include "heretothere.h"
#include "locations.h"
#include "traveltime.h"

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
	APP_LOG(APP_LOG_LEVEL_DEBUG, "in_received_handler");
	if (dict_find(iter, LOCATIONS_KEY)) {
		locations_in_received_handler(iter);
	}else if (dict_find(iter, TRAVELTIME_KEY)) {
		traveltime_in_received_handler(iter);
	}
}

static void in_dropped_handler(AppMessageResult reason, void *context) {
}

static void out_sent_handler(DictionaryIterator *sent, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "out_send_handler");
	if (dict_find(sent, LOCATIONS_KEY)) {
		locations_out_sent_handler(sent);
	}else if (dict_find(sent, TRAVELTIME_KEY)) {
		traveltime_out_sent_handler(sent);
	}
}

static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "out_failed_handler");
	
	char* message;
	switch(reason) {
		case APP_MSG_OK:
			message = "APP_MSG_OK";
			break;
		case APP_MSG_SEND_TIMEOUT:
			message = "APP_MSG_SEND_TIMEOUT";
			break;
		case APP_MSG_SEND_REJECTED:
			message = "APP_MSG_SEND_REJECTED";
			break;
		case APP_MSG_NOT_CONNECTED:
			message = "APP_MSG_NOT_CONNECTED";
			break;
		case APP_MSG_APP_NOT_RUNNING:
			message = "APP_MSG_APP_NOT_RUNNING";
			break;
		case APP_MSG_INVALID_ARGS:
			message = "APP_MSG_INVALID_ARGS";
			break;
		case APP_MSG_BUFFER_OVERFLOW:
			message = "APP_MSG_BUFFER_OVERFLOW";
			break;
		case APP_MSG_ALREADY_RELEASED:
			message = "APP_MSG_ALREADY_RELEASED";
			break;
		case APP_MSG_CALLBACK_ALREADY_REGISTERED:
			message = "APP_MSG_CALLBACK_ALREADY_REGISTERED";
			break;
		case APP_MSG_CALLBACK_NOT_REGISTERED:
			message = "APP_MSG_CALLBACK_NOT_REGSITERED";
			break;
		case APP_MSG_OUT_OF_MEMORY:
			message = "APP_MSG_OUT_OF_MEMORY";
			break;
		case APP_MSG_CLOSED:
			message = "APP_MSG_CLOSED";
			break;
		case APP_MSG_INTERNAL_ERROR:
			message = "APP_MSG_INTERNAL_ERROR";
			break;
		default:
			message = "OTHER";
	}
	APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox failed!");
	APP_LOG(APP_LOG_LEVEL_ERROR, message);
	
	if (dict_find(failed, LOCATIONS_KEY)) {
		locations_out_failed_handler(failed, reason);
	}else if (dict_find(failed, TRAVELTIME_KEY)) {
		traveltime_out_failed_handler(failed, reason);
	}
}