#pragma once

#include <string>

namespace ctx_log::internal {

	struct LevelColor {
		std::string icon;
		std::string color;
	};

	struct Fields {
		std::string module;
		std::string caller;
	};

	struct Colors {
		LevelColor level[7];
		Fields fields;
	};

	extern Colors colors;
	constexpr const char* reset = "\u001b[0m";

	constexpr std::string Colorize(const std::string& s, std::string& color) {
		return color + s + reset;
	}
}
