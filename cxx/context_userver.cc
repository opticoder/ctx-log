#include <userver/engine/task/inherited_variable.hpp>

#include "context.h"


namespace ctx_log {
	namespace internal {

//		TODO: sync
		userver::engine::TaskInheritedVariable<CtxFields> ctxValues;

		class UserverContext : public ContextAdapter {
			void Set(const std::string& key, const std::string& value) noexcept override {
				CtxFields fields;
				auto current = ctxValues.GetOptional();
				if (nullptr != current) {
					fields = *current;
				} else {
					fields = staticFields;
				}
				fields[key] = value;
				ctxValues.Set(fields);
			}

			const CtxFields& Get() const noexcept override {
					auto values = ctxValues.GetOptional();
					if (nullptr == values) {
						ctxValues.Set(staticFields);
						return ctxValues.Get();
					}
					return *values;
			}

			std::shared_ptr<CtxFields> Backup() noexcept override {
				auto fields = ctxValues.GetOptional();
				if (nullptr == fields) {
					return std::make_shared<CtxFields>(staticFields);
				}
				return std::make_shared<CtxFields>(*fields);
			}

			void Restore(const std::shared_ptr<CtxFields>& fields) noexcept override {
				ctxValues.Set(*fields);
			}
		};
	}

	ContextAdapter* InitUserverContext() {
		return new internal::UserverContext();
	}
}
