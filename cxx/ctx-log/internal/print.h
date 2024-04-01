#pragma once

#include <string>
#include <utility>


namespace ctx_log::internal {
	inline void printFields(const char* begin_sep, const char* kv_sep, std::string& s, CtxFields& f) {
		const char* end_sep = "\"";
		for (auto& [key, value]: f) {
			s.append(begin_sep);
			s.append(key);
			s.append(kv_sep);
			s.append(value);
			s.append(end_sep);
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
	std::string& sprint(std::string& s, F&& first, R&&... rest) {
		using ctx_log::internal::to_string;
		using std::to_string;
		s += to_string(std::forward<F>(first)) + " ";
		if constexpr (sizeof...(rest) > 0) {
			sprint(s, std::forward<R>(rest)...);
		} else {
			s.pop_back();
		}
		return s;
	}
}
