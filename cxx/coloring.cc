#include <sstream>
#include <string>

#include "fkYAML/node.hpp"

#include "coloring.h"

const char* cfg =
#include "generated/colors.yaml.inc"
;

namespace ctx_log::internal {
	Colors colors;

	std::string readColor(fkyaml::node& root, const char* type, const char* level) {
		return root[type][level].get_value_ref<std::string&>();
	}

	void loadColors() {
		std::istringstream colors_yaml(cfg);
		fkyaml::node root = fkyaml::node::deserialize(colors_yaml);

		colors.fields.module = root["fields"]["module"].get_value_ref<std::string&>();
		colors.fields.caller = root["fields"]["caller"].get_value_ref<std::string&>();

		colors.level[0].icon = readColor(root, "icons", "trace");
		colors.level[1].icon = readColor(root, "icons", "debug");
		colors.level[2].icon = readColor(root, "icons", "info");
		colors.level[3].icon = readColor(root, "icons", "warn");
		colors.level[4].icon = readColor(root, "icons", "error");
		colors.level[5].icon = readColor(root, "icons", "crit");
		colors.level[6].icon = readColor(root, "icons", "fatal");

		colors.level[0].color = readColor(root, "levels", "trace");
		colors.level[1].color = readColor(root, "levels", "debug");
		colors.level[2].color = readColor(root, "levels", "info");
		colors.level[3].color = readColor(root, "levels", "warn");
		colors.level[4].color = readColor(root, "levels", "error");
		colors.level[5].color = readColor(root, "levels", "crit");
		colors.level[6].color = readColor(root, "levels", "fatal");
	}
}
