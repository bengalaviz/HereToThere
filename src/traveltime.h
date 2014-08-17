#pragma once

void traveltime_init(void);
void traveltime_deinit(void);
void traveltime_show(char *location);

void traveltime_in_received_handler(DictionaryIterator *iter);
void traveltime_out_sent_handler(DictionaryIterator *sent);
void traveltime_out_failed_handler(DictionaryIterator *failed, AppMessageResult reason);