#include <string>
#include <memory>

#include "../ctx-log.h"


namespace ctx_log {
	namespace internal {
		Config config;
	}

	ContextLogger getLogger(const char* name, ContextAdapter* ctx) {
		quill::Logger* logger;
		try {
			logger = quill::get_logger(name);
		} catch (std::exception&) {
			logger = quill::create_logger(name);
			logger->set_log_level(quill::get_logger()->log_level());
		}
		return {logger, ctx, nullptr};
	};

	std::string setLogger(const Config& config) {
		internal::config = config;
		if (nullptr == internal::config.context) {
			internal::config.context = InitThreadsContext();
		}

		auto handler = quill::stdout_handler();
		if (internal::config.JSON) {
			handler->set_pattern("{{\"time\":\"%(ascii_time)\"%(message)}}", "%Y-%m-%dT%H:%M:%S.%QmsZ", quill::Timezone::GmtTime);
		} else {
			handler->set_pattern("%(message)", "", quill::Timezone::GmtTime);
		}

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

	ContextLogger ContextLogger::withCtxFields() {
		return {logger, nullptr, internal::config.context->Backup()};
	}

	ContextLogger ContextLogger::withCtxFields(ContextAdapter& explicit_ctx) {
		return {logger, &explicit_ctx, nullptr};
	}

	void ContextLogger::setCtxField(const std::string& key, const std::string& value) {
		if (ctx) {
			ctx->Set(key, value);
			return;
		}
		internal::config.context->Set(key, value);
	}
}
