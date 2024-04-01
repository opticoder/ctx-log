#include <sstream>
#include <string>

#include <fkYAML/node.hpp>

#include "coloring.h"
#include "level.h"


const char* cfg =
#include "../../generated/colors.yaml.inc"
;

namespace ctx_log::internal {
	Colors colors;

	void loadColors() {
		std::istringstream colors_yaml(cfg);
		fkyaml::node root = fkyaml::node::deserialize(colors_yaml);

		colors.fields.module = root["fields"]["module"].get_value_ref<std::string&>();
		colors.fields.caller = root["fields"]["caller"].get_value_ref<std::string&>();

		for (auto level = 0; level < levelsCount; level++) {
			auto levelName = level2lowercase(static_cast<Level>(level));
			colors.level[level].icon = root["icons"][levelName].get_value_ref<std::string&>();
			colors.level[level].color = root["levels"][levelName].get_value_ref<std::string&>();
		}
	}
}
