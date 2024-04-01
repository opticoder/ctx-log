#include <userver/engine/task/inherited_variable.hpp>

#include "../context.h"


namespace ctx_log::internal {
//		TODO: sync
	userver::engine::TaskInheritedVariable<CtxFields> ctxValues;

	class UserverContext final : public ContextAdapter {
		[[nodiscard]] const CtxFields &Get() const noexcept override {
			auto values = ctxValues.GetOptional();
			if (nullptr == values) {
				ctxValues.Set(CtxFields());
				return ctxValues.Get();
			}
			return *values;
		}

		void Set(const std::string &key, const std::string &value) noexcept override {
			CtxFields fields;
			auto current = ctxValues.GetOptional();
			if (nullptr != current) {
				fields = *current;
			}
			fields[key] = value;
			ctxValues.Set(fields);
		}

		[[nodiscard]] std::shared_ptr<CtxFields> Backup() const noexcept override {
			auto fields = ctxValues.GetOptional();
			if (nullptr == fields) {
				return std::make_shared<CtxFields>(/*staticFields*/);
			}
			return std::make_shared<CtxFields>(*fields);
		}

		void Restore(const std::shared_ptr<CtxFields> &fields) noexcept override {
			ctxValues.Set(*fields);
		}
	};
}

namespace ctx_log {
	ContextAdapter* InitUserverContext() {
		return new internal::UserverContext();
	}
}
