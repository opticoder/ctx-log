#include <string>
#include <memory>

#include "ctx-log.h"


namespace ctx_log {
	namespace internal {
		Config config;
		void loadColors();
	}

	ContextLogger getLogger(char const* name) {
		return ContextLogger(name);
	};

	std::string setLogger(Config const& config) {
		internal::config = config;
		ContextAdapter* context;
		if (nullptr == config.context) {
			context = internal::initThreadsContext();
		} else {
			context = config.context;
		}
		internal::setLoggerContext(context);
		context->SetStaticFields(config.staticFields);

		std::shared_ptr<quill::Handler> handler = quill::stdout_handler();
		if (internal::config.JSON) {
			handler->set_pattern("{{\"time\":\"%(ascii_time)\"%(message)}}", "%Y-%m-%dT%H:%M:%S.%QmsZ", quill::Timezone::GmtTime);
		} else {
			handler->set_pattern("%(message)", "", quill::Timezone::GmtTime);
		}

//		std::shared_ptr<quill::Handler> handler = std::make_shared<internal::CustomHandler>();
//		auto handler = std::make_shared<internal::CustomHandler>();
//		auto handler = std::make_shared<quill::ConsoleHandler>(std::string{"stdout"}, stdout, quill::ConsoleColours{});
//		handler->formatter
		quill::Config cfg;
		cfg.default_handlers.emplace_back(handler);
		quill::configure(cfg);
		quill::start();

		setLevel(config.level);

		internal::loadColors();
		return "";
	}

	void ContextLogger::setLevel(Level level) {
		logger->set_log_level(ctx_log::internal::quillLevel(level));
	}

	void setLevel(Level level) {
		for(auto logger : quill::get_all_loggers()) {
			logger.second->set_log_level(ctx_log::internal::quillLevel(level));
		}
	}

	ContextHolder ContextLogger::withCtxFields() {
		return ContextHolder(internal::getLoggerContext()->Backup());
	}

	void ContextLogger::setCtxField(const ContextHolder& ctx, const std::string& key, const std::string& value) {
		internal::getLoggerContext()->Set(key, value);
	}

	ContextLogger::ContextLogger(char const* name) {
		try {
			logger = quill::get_logger(name);
		} catch (std::exception&) {
			logger = quill::create_logger(name);
			logger->set_log_level(quill::get_logger()->log_level());
		}
	};

	namespace internal {
		ContextAdapter* context;

		ContextAdapter* getLoggerContext() {
			return context;
		}

		void setLoggerContext(ContextAdapter* ctx) {
			context = ctx;
		}

		class CustomFormatter : public quill::PatternFormatter {
		public:
			CustomFormatter() : PatternFormatter("", "", quill::Timezone::GmtTime) {}
			QUILL_NODISCARD QUILL_ATTRIBUTE_HOT quill::fmt_buffer_t const& format(
					std::chrono::nanoseconds timestamp, std::string_view thread_id, std::string_view thread_name,
					std::string_view process_id, std::string_view logger_name, std::string_view log_level,
					quill::MacroMetadata const& macro_metadata,
					std::vector<std::pair<std::string, quill::transit_event_fmt_buffer_t>> const& structured_kvs,
					quill::transit_event_fmt_buffer_t const& log_msg) /*override*/ {
				buf.clear();
				buf.append(log_msg.begin(), log_msg.end());
				return buf;
			}
		private:
			quill::fmt_buffer_t buf;
		};
		class CustomHandler : public quill::ConsoleHandler {
		public:
			CustomHandler() : quill::ConsoleHandler(std::string{"stdout"}, stdout, quill::ConsoleColours{}) {
				_formatter = std::make_unique<CustomFormatter>();
			}
//			~CustomHandler() override = default;
		};
	}
}
