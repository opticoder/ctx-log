#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <iostream>


namespace ctx_log {
	using CtxFields = std::unordered_map<std::string, std::string>;

	class ContextAdapter {
	private:
//		TODO: iterator
// std::span<const std::pair<const std::string&, const std::string&>>
		[[nodiscard]] virtual const CtxFields& Get() const noexcept = 0;
		virtual void Set(const std::string& key, const std::string& value) noexcept = 0;
		[[nodiscard]] virtual std::shared_ptr<CtxFields> Backup() const noexcept = 0;
		virtual void Restore(const std::shared_ptr<CtxFields>& fields) noexcept = 0;

		friend class ContextLogger;
	};

	class Context final : public ContextAdapter {
	public:
		[[nodiscard]] const CtxFields& Get() const noexcept override {
			return fields;
		}
		void Set(const std::string& key, const std::string& value) noexcept override {
			fields[key] = value;
		}
		[[nodiscard]] std::shared_ptr<CtxFields> Backup() const noexcept override { return nullptr; }
		void Restore(const std::shared_ptr<CtxFields>&) noexcept override {}
	private:
		CtxFields fields;
	};

	ContextAdapter* InitThreadsContext();
	ContextAdapter* InitUserverContext();
}
