#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void nicolive_mbox_error(const char *message);
void nicolive_mbox_warn(const char *message);
void nicolive_mbox_info(const char *message);

void nicolive_msg_error(bool available_gui, const char *gui_message,
		const char *cui_message);
void nicolive_msg_warn(bool available_gui, const char *gui_message,
		const char *cui_message);
void nicolive_msg_info(bool available_gui, const char *gui_message,
		const char *cui_message);

// void nicolive_streaming_click(void);

#ifdef __cplusplus
}
#endif
