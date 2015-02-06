#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void nicolive_mbox_error(const char *message);
void nicolive_mbox_warn(const char *message);
void nicolive_mbox_info(const char *message);

void nicolive_streaming_click(void);

#ifdef __cplusplus
}
#endif
