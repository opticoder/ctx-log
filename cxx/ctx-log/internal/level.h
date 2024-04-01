#pragma once

#include <quill/Quill.h>

#include "../level.h"


namespace ctx_log::internal {
	consteval const char* level2text(Level level) {
		switch (level) {
			using enum Level;
			case TRACE: return "TRACE";
			case DEBUG: return "DEBUG";
			case INFO:  return "INFO ";
			case WARN:  return "WARN ";
			case ERROR: return "ERROR";
			case CRIT:  return "CRIT ";
			case FATAL: return "FATAL";
		}
	}

	constexpr const char* level2lowercase(Level level) {
		switch (level) {
			using enum Level;
			case TRACE: return "trace";
			case DEBUG: return "debug";
			case INFO:  return "info";
			case WARN:  return "warn";
			case ERROR: return "error";
			case CRIT:  return "crit";
			case FATAL: return "fatal";
		}
	}

	constexpr quill::LogLevel quillLevel(const Level level) {
		switch (level) {
			using enum Level;
			case TRACE: return quill::LogLevel::TraceL1;
			case DEBUG: return quill::LogLevel::Debug;
			case INFO:  return quill::LogLevel::Info;
			case WARN:  return quill::LogLevel::Warning;
			case ERROR: return quill::LogLevel::Error;
			case CRIT:
			case FATAL: return quill::LogLevel::Critical;
		}
	}
}
