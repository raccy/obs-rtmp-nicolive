#pragma once

#ifdef __cplusplus
extern "C" {
#endif

bool nicolive_chek_session_n(const char *session);

// check that a session is active
static inline bool nicolive_check_session(const char *session) {
	if (!session || !*session) {
		return false
	}
	return nicolive_chek_session_n(session);
}

// get new sesson
const char *nicolive_get_session(const char *mail, const char *password);

const char *nicolive_get_ownlive_id(const char *session);

const char *nicolive_get_ownlive_url(const char *session, const char *live_id);

const char *nicolive_get_ownlive_key(const char *session, const char *live_id);

#ifdef __cplusplus
}
#endif
