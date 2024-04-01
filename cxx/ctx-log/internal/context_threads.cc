#include "../context.h"


namespace ctx_log::internal {
	thread_local std::shared_ptr<CtxFields> ctxValues;

	class ThreadsContext final : public ContextAdapter {
		[[nodiscard]] const CtxFields& Get() const noexcept override {
			return *ctxValues;
		}

		void Set(const std::string& key, const std::string& value) noexcept override {
			ctxValues->insert_or_assign(key, value);
		}

		[[nodiscard]] std::shared_ptr<CtxFields> Backup() const noexcept override {
			return std::make_shared<CtxFields>(*ctxValues);
		}

		void Restore(const std::shared_ptr<CtxFields>& fields) noexcept override {
			ctxValues = fields;
		}
	};
}

namespace ctx_log {
	ContextAdapter* InitThreadsContext() {
		internal::ctxValues = std::make_shared<CtxFields>();
		return new internal::ThreadsContext();
	}
}
