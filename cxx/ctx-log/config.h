#pragma once

#include "context.h"
#include "level.h"


namespace ctx_log {
	struct Config {
		bool            JSON;
		Level           level;
		CtxFields       staticFields;
		bool            noColors;
		bool            noIcons;

		char*           messageField;
		char*           levelField;
		char*           timeField;
		char*           callerField;
		char*           moduleField;
		ContextAdapter* context;
	};

	namespace internal {
		extern Config config;
	}
}
