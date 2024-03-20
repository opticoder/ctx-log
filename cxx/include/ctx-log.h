#pragma once

#include <source_location>
#include <filesystem>
#include <memory>

#include <fmt/printf.h>
#include <quill/Quill.h>
#include <quill/detail/LogMacros.h>

#include "context.h"
#include "coloring.h"


namespace ctx_log {
	enum class Level {
		TRACE,
		DEBUG,
		INFO,
		WARN,
		ERROR,
		CRIT,
		FATAL,
	};

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

		consteval const char* level2text(Level level) {
			switch (level) {
				case Level::TRACE:
					return "TRACE";
				case Level::DEBUG:
					return "DEBUG";
				case Level::INFO:
					return "INFO ";
				case Level::WARN:
					return "WARN ";
				case Level::ERROR:
					return "ERROR";
				case Level::CRIT:
					return "CRIT ";
				case Level::FATAL:
					return "FATAL";
			}
		}

		consteval const char* level2lowercase(Level level) {
			switch (level) {
				case Level::TRACE:
					return "trace";
				case Level::DEBUG:
					return "debug";
				case Level::INFO:
					return "info";
				case Level::WARN:
					return "warn";
				case Level::ERROR:
					return "error";
				case Level::CRIT:
					return "crit";
				case Level::FATAL:
					return "fatal";
			}
		}

		constexpr quill::LogLevel quillLevel(const Level level) {
			switch (level) {
				case Level::TRACE:
					return quill::LogLevel::TraceL1;
				case Level::DEBUG:
					return quill::LogLevel::Debug;
				case Level::INFO:
					return quill::LogLevel::Info;
				case Level::WARN:
					return quill::LogLevel::Warning;
				case Level::ERROR:
					return quill::LogLevel::Error;
				case Level::CRIT:
				case Level::FATAL:
					return quill::LogLevel::Critical;
			}
		}

		inline std::string to_string(std::string s) {
			return s;
		}
		inline std::string to_string(char c) {
			std::string s(1, c);
			return s;
		}
		template<typename F, typename... R>
		std::string& sprint(std::string& s, F&& first, R&&... rest)
		{
			using internal::to_string;;
			using std::to_string;
			s += to_string(std::forward<F>(first))+" ";
			if constexpr (sizeof...(rest) > 0) {
				sprint(s, std::forward<R>(rest)...);
			} else {
				s.pop_back();
			}
			return s;
		}
	}

	class ContextLogger {
	public:
		static constexpr const char* format="{} {} {} {} {}{}";
		// myDebug() = default;
		// myDebug(myDebug const &) = delete;
		// myDebug & operator = (myDebug const &) = delete;
		// myDebug & operator = (myDebug &&) = delete;
		// myDebug(myDebug && dc) noexcept

//		inline void quill::flush()
		ContextHolder withCtxFields();
		void setCtxField(const ContextHolder& ctx, const std::string& key, const std::string& value);
//		ContextHolder* setCtxField(const std::string& key, const std::string& value);

		bool isEnabledFor(Level level);
		// LogFor(level Level, fmt func())
		void setLevel(Level level);

		struct Logger {

			Logger(ContextLogger &l, const std::source_location &src) : logger(l.logger) {
				filepath = src.file_name();
				line = src.line();
			}

			template<Level level, typename... Args>
			void log_printf(const char* fmt, Args&&... args) {
				if (logger->log_level() > internal::quillLevel(level))
					return;
				log<level>(fmt::sprintf(fmt, std::forward<Args>(args)...));
			}

			template<Level level, typename... Args>
			void log_print(Args&&... args) {
				if (logger->log_level() > internal::quillLevel(level))
					return;
				std::string msg;
				log<level>(internal::sprint(msg, std::forward<Args>(args)...));
			}

			template<Level level>
			void log(const std::string& msg) {
//				auto msg = log_sprintf(fmt, std::forward<Args>(args)...);
//				internal::log(Level::DEBUG, filename.c_str(), line, msg.c_str());

				auto filename = filepath.filename();
				auto dir = filepath.parent_path().filename();

				auto ctxFields = internal::getLoggerContext()->Get();
				std::string fields;

				if (internal::config.JSON) {
					fields = std::string(",") + "\"level\":\"" + internal::level2lowercase(level) + "\"";
					fields += ",\"module\":\"" + logger->name() + "\"";
					fields += ",\"caller\":\"" + std::string(dir / filename) + ":" + std::to_string(line) + "\"";
					fields += ",\"msg\":\"" + msg + "\"";
					for (auto& kv: ctxFields) {
						fields += ",\"" + kv.first + "\":\"" + kv.second + "\"";
					};

					QUILL_LOGGER_CALL(QUILL_LIKELY, logger, internal::quillLevel(level),
									  "{}", fields);
				} else {
					for (auto& kv: ctxFields) {
						fields += " " + kv.first + "=\"" + kv.second + "\"";
					};

					using internal::Colorize;
					using internal::colors;
					QUILL_LOGGER_CALL(QUILL_LIKELY, logger, internal::quillLevel(level),
									  format,
									  colors.level[static_cast<int>(level)].icon,
									  Colorize(internal::level2text(level),
											   colors.level[static_cast<int>(level)].color),
									  msg,
									  Colorize("[" + logger->name() + "]", colors.fields.module),
									  Colorize("[" + std::string(dir / filename) + ":" + std::to_string(line) + "]",
											   colors.fields.caller),
									  fields
					);
				}
			}

			template<typename... Args>
			void trace(Args&&... args) {
				log_print<Level::TRACE>(std::forward<Args>(args)...);
			}

			template<typename... Args>
			void tracef(const char* fmt, Args&&... args) {
				log_printf<Level::TRACE>(fmt, std::forward<Args>(args)...);
			}

			template<typename... Args>
			void debug(Args&&... args) {
				log_print<Level::DEBUG>(std::forward<Args>(args)...);
			}

			template<typename... Args>
			void debugf(const char* fmt, Args&&... args) {
				log_printf<Level::DEBUG>(fmt, std::forward<Args>(args)...);
			}

			template<typename... Args>
			void info(Args&&... args) {
				log_print<Level::INFO>(std::forward<Args>(args)...);
			}

			template<typename... Args>
			void infof(const char* fmt, Args&&... args) {
				log_printf<Level::INFO>(fmt, std::forward<Args>(args)...);
			}

			template<typename... Args>
			void warn(Args&&... args) {
				log_print<Level::WARN>(std::forward<Args>(args)...);
			}

			template<typename... Args>
			void warnf(const char* fmt, Args&&... args) {
				log_printf<Level::WARN>(fmt, std::forward<Args>(args)...);
			}

			template<typename... Args>
			void error(Args&&... args){
				log_print<Level::ERROR>(std::forward<Args>(args)...);
			}

			template<typename... Args>
			void errorf(const char* fmt, Args&&... args) {
				log_printf<Level::ERROR>(fmt, std::forward<Args>(args)...);
			}

			template<typename... Args>
			void crit(Args&&... args) {
				log_print<Level::CRIT>(std::forward<Args>(args)...);
			}

			template<typename... Args>
			void critf(const char* fmt, Args&&... args) {
				log_printf<Level::CRIT>(fmt, std::forward<Args>(args)...);
			}

			template<typename... Args>
			void fatal(Args&&... args) {
				log_print<Level::FATAL>(std::forward<Args>(args)...);
				std::exit(1);
			}

			template<typename... Args>
			void fatalf(const char* fmt, Args&&... args) {
				log_printf<Level::FATAL>(fmt, std::forward<Args>(args)...);
				std::exit(1);
			}
		private:
			std::filesystem::path filepath;
			unsigned line;
			quill::Logger* logger;
		};

		Logger operator()(int param = 0, const std::source_location &source = std::source_location::current()) noexcept {
			return Logger(*this, source);
		}
	private:
		friend ContextLogger getLogger(char const* name);
		explicit ContextLogger(char const* name);
		quill::Logger* logger;
	};

// withFields
// TODO: withException

	// TODO: reset context
	ContextLogger getLogger(char const* name = nullptr);
	std::string setLogger(Config const& config);
	void setLevel(Level level);
}
