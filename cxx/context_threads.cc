#include "context.h"


namespace ctx_log {
	namespace internal {
		thread_local std::shared_ptr<CtxFields> ctxValues;

		class ThreadsContext : public ContextAdapter {
			void SetStaticFields(const CtxFields fields) noexcept override { ctxValues = std::make_shared<CtxFields>(fields); }
			const CtxFields& Get() const noexcept override {
				return *ctxValues;
			}

			void Set(const std::string& key, const std::string& value) noexcept override {
				ctxValues->insert_or_assign(key, value);
			}

			std::shared_ptr<CtxFields> Backup() noexcept override {
				return std::make_shared<CtxFields>(*ctxValues);
			}

			void Restore(const std::shared_ptr<CtxFields>& fields) noexcept override {
				ctxValues = fields;
			}
		};

		ContextAdapter *initThreadsContext() {
			return new ThreadsContext();
		}
	}
}
