#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <iostream>


namespace ctx_log {

	using CtxFields = std::unordered_map<std::string, std::string>;

	class ContextAdapter {
	public:
		virtual void SetStaticFields(const CtxFields fields) noexcept { staticFields = std::move(fields); }
//		TODO: iterator
	protected:
		CtxFields staticFields;
	private:
		virtual const CtxFields& Get() const noexcept = 0;
		virtual void Set(const std::string& key, const std::string& value) noexcept = 0;
		virtual std::shared_ptr<CtxFields> Backup() noexcept = 0;
		virtual void Restore(const std::shared_ptr<CtxFields>& fields) noexcept = 0;

		friend class ContextLogger;
		friend class ContextHolder;
	};

	namespace internal {
		ContextAdapter* getLoggerContext();
		void setLoggerContext(ContextAdapter* ctx);

		ContextAdapter* initThreadsContext();
	}

	class ContextHolder {
	public:
		explicit ContextHolder(const std::shared_ptr<CtxFields>& values) noexcept : fields(values) {}

		~ContextHolder() noexcept {
			internal::getLoggerContext()->Restore(fields);
		}
	private:
		std::shared_ptr<CtxFields> fields;
	};

	ContextAdapter* InitUserverContext();
}
