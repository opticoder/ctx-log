#pragma once

#include <string>

#include "../level.h"


namespace ctx_log::internal {
	constinit const int levelsCount = 1 + static_cast<int>(Level::FATAL);

	struct LevelColor {
		std::string icon;
		std::string color;
	};

	struct Fields {
		std::string module;
		std::string caller;
	};

	struct Colors {
		LevelColor level[levelsCount];
		Fields fields;
	};

	extern Colors colors;
	constexpr const char* reset = "\u001b[0m";

	constexpr std::string Colorize(const std::string& s, std::string& color) {
		return color + s + reset;
	}

	void loadColors();
}
