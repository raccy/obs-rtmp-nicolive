#include "nicolive.h"
#include <stdbool.h>
#include <string.h>
#include <obs-module.h>
#include "nicolive-log.h"
#include "nicolive-operation.h"

#define START_STREAMING_HOTKEY_NAME "OBSBasic.StartStreaming"
// #define STOP_STREAMING_HOTKEY_NAME "OBSBasic.StopStreaming"
#define STOP_STREAMING_HOTKEY_NAME "OBSBasic.ForceStopStreaming"

inline static bool nicolive_operation_click_enum_func(
		void *data, obs_hotkey_id id, obs_hotkey_t *key)
{
	nicolive_log_debug_call_func();
	const char *target = data;
	const char *name = obs_hotkey_get_name(key);
	if (strcmp(name, target) == 0) {
		nicolive_log_debug("click hotkey: %s", name);
		obs_hotkey_trigger_routed_callback(id, true);
		return false;
	}
	return true;
}

void nicolive_streaming_start(void)
{
	nicolive_log_debug_call_func();
	nicolive_log_info("click start streaming hotkey");
	obs_enum_hotkeys(nicolive_operation_click_enum_func,
			START_STREAMING_HOTKEY_NAME);
}

void nicolive_streaming_stop(void)
{
	nicolive_log_debug_call_func();
	nicolive_log_info("click stop streaming hotkey");
	obs_enum_hotkeys(nicolive_operation_click_enum_func,
			STOP_STREAMING_HOTKEY_NAME);
}
