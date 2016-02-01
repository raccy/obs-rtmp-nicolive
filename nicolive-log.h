#pragma once

#include <stdbool.h>
#include <util/base.h>

#ifndef __func__
#define __func__ __FUNCTION__
#endif
#define nicolive_log_error(format, ...) \
		blog(LOG_ERROR, "[nicolive] " format, ##__VA_ARGS__)
#define nicolive_log_warn(format, ...) \
		blog(LOG_WARNING, "[nicolive] " format, ##__VA_ARGS__)
#define nicolive_log_info(format, ...) \
		blog(LOG_INFO, "[nicolive] " format, ##__VA_ARGS__)

#ifdef _DEBUG
#define nicolive_log_debug(format, ...) \
		blog(LOG_DEBUG, "[nicolive] " format, ##__VA_ARGS__)
#define nicolive_log_debug_call_func() \
		nicolive_log_debug("call func: %s", __func__)
#else
#define nicolive_log_debug(format, ...)
#define nicolive_log_debug_call_func()
#endif

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

#ifdef __cplusplus
}
#endif
