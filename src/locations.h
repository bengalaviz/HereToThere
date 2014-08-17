#pragma once

void locations_init(void);
void locations_deinit(void);
void locations_show();

void locations_in_received_handler(DictionaryIterator *iter);
void locations_out_sent_handler(DictionaryIterator *sent);
void locations_out_failed_handler(DictionaryIterator *failed, AppMessageResult reason);