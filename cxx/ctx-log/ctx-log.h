#pragma once

#include <source_location>
#include <filesystem>
#include <memory>
#include <utility>

#include <fmt/printf.h>
#include <quill/Quill.h>
#include <quill/detail/LogMacros.h>

#include "context.h"
#include "config.h"

#include "internal/level.h"
#include "internal/print.h"
#include "internal/coloring.h"


namespace ctx_log {
	class ContextLogger;

	std::string setLogger(const Config& config);
	ContextLogger getLogger(const char* name = nullptr, ContextAdapter* ctx = nullptr);
	void setLevel(Level level);

	class ContextLogger {
	public:
		static constexpr const char* format="{} {} {} {} {}{}";
		// myDebug() = default;
		// myDebug(myDebug const &) = delete;
		// myDebug & operator = (myDebug const &) = delete;
		// myDebug & operator = (myDebug &&) = delete;
		// myDebug(myDebug && dc) noexcept

//		inline void quill::flush()

// TODO: withFields withException
// TODO: reset context

		ContextLogger withCtxFields();
		ContextLogger withCtxFields(ContextAdapter& explicit_ctx);
		void setCtxField(const std::string& key, const std::string& value);

		bool isEnabledFor(Level level) const;
		// LogFor(level Level, fmt func())
		void setLevel(Level level);

		struct Logger {

			template<typename... Args>
			void trace(Args&&... args) const {
				log_print<Level::TRACE>(std::forward<Args>(args)...);
			}

			template<typename... Args>
			void tracef(const char* fmt, Args&&... args) const {
				log_printf<Level::TRACE>(fmt, std::forward<Args>(args)...);
			}

			template<typename... Args>
			void debug(Args&&... args) const {
				log_print<Level::DEBUG>(std::forward<Args>(args)...);
			}

			template<typename... Args>
			void debugf(const char* fmt, Args&&... args) const {
				log_printf<Level::DEBUG>(fmt, std::forward<Args>(args)...);
			}

			template<typename... Args>
			void info(Args&&... args) const {
				log_print<Level::INFO>(std::forward<Args>(args)...);
			}

			template<typename... Args>
			void infof(const char* fmt, Args&&... args) const {
				log_printf<Level::INFO>(fmt, std::forward<Args>(args)...);
			}

			template<typename... Args>
			void warn(Args&&... args) const {
				log_print<Level::WARN>(std::forward<Args>(args)...);
			}

			template<typename... Args>
			void warnf(const char* fmt, Args&&... args) const {
				log_printf<Level::WARN>(fmt, std::forward<Args>(args)...);
			}

			template<typename... Args>
			void error(Args&&... args) const {
				log_print<Level::ERROR>(std::forward<Args>(args)...);
			}

			template<typename... Args>
			void errorf(const char* fmt, Args&&... args) const {
				log_printf<Level::ERROR>(fmt, std::forward<Args>(args)...);
			}

			template<typename... Args>
			void crit(Args&&... args) const {
				log_print<Level::CRIT>(std::forward<Args>(args)...);
			}

			template<typename... Args>
			void critf(const char* fmt, Args&&... args) const {
				log_printf<Level::CRIT>(fmt, std::forward<Args>(args)...);
			}

			template<typename... Args>
			void fatal(Args&&... args) const {
				log_print<Level::FATAL>(std::forward<Args>(args)...);
				std::exit(1);
			}

			template<typename... Args>
			void fatalf(const char* fmt, Args&&... args) const {
				log_printf<Level::FATAL>(fmt, std::forward<Args>(args)...);
				std::exit(1);
			}
		private:
			std::filesystem::path filepath;
			unsigned line;
			quill::Logger* logger;
			const ContextAdapter* ctx;

			Logger(quill::Logger* logger, const ContextAdapter* ctx, const std::source_location& src) noexcept : logger(logger), ctx(ctx) {
				filepath = src.file_name();
				line = src.line();
			}
			friend class ContextLogger;

			template<Level level, typename... Args>
			void log_printf(const char* fmt, Args&&... args) const {
				if (logger->log_level() > internal::quillLevel(level))
					return;
				log<level>(fmt::sprintf(fmt, std::forward<Args>(args)...));
			}

			template<Level level, typename... Args>
			void log_print(Args&&... args) const {
				if (logger->log_level() > internal::quillLevel(level))
					return;
				std::string msg;
				log<level>(internal::sprint(msg, std::forward<Args>(args)...));
			}

			template<Level level>
			void log(const std::string& msg) const {
				auto filename = filepath.filename();
				auto dir = filepath.parent_path().filename();

				auto ctxFields = ctx->Get();
				std::string fields;

				if (internal::config.JSON) {
					fields = std::string(",") + "\"level\":\"" + internal::level2lowercase(level) + "\"";
					fields += ",\"module\":\"" + logger->name() + "\"";
					fields += ",\"caller\":\"" + std::string(dir / filename) + ":" + std::to_string(line) + "\"";
					fields += ",\"msg\":\"" + msg + "\"";

					internal::printFields(",\"", "\":\"", fields, internal::config.staticFields);
					internal::printFields(",\"", "\":\"", fields, ctxFields);

					QUILL_LOGGER_CALL(QUILL_LIKELY, logger, internal::quillLevel(level),
									  "{}", fields);
				} else {
					internal::printFields(" ", "=\"", fields, internal::config.staticFields);
					internal::printFields(" ", "=\"", fields, ctxFields);

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
		};

		Logger operator()(int param = 0, const std::source_location &src = std::source_location::current()) noexcept {
			if (ctx) {
				return {logger, ctx, src};
			}
			return {logger, internal::config.context, src};
		}
		~ContextLogger() noexcept {
			if (fields) {
				internal::config.context->Restore(fields);
			}
		}
	private:
		quill::Logger* logger;
		ContextAdapter* ctx;
		std::shared_ptr<CtxFields> fields;

		friend ContextLogger getLogger(const char* name, ContextAdapter* ctx);
		ContextLogger(quill::Logger* logger, ContextAdapter* ctx, std::shared_ptr<CtxFields> fields) noexcept
		 : logger(logger), ctx(ctx), fields(std::move(fields)) {}
	};
}
