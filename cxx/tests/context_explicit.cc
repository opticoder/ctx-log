#include <ctx-log/ctx-log.h>

using ctx_log::Context;
auto logging = ctx_log::getLogger("module1");


void level3(Context ctx) {
	auto logger = logging.withCtxFields(ctx);
	logger().debug("level3 before set ctx");
	logger.setCtxField("level3", "val");
	logger().debug("level3 after set ctx");
}

void level2(Context ctx) {
	auto logger = logging.withCtxFields(ctx);
	logger().debug("level2 before set ctx");
	logger.setCtxField("level2", "val");
	logger().debug("level2 after set ctx");
	level3(ctx);
	logger().debug("level2 after level3");
}

void level1(Context ctx) {
	auto logger = logging.withCtxFields(ctx);
	logger().debug("level1 before set ctx");
	logger.setCtxField("level1", "val");
	logger().debug("level1 after set ctx");
	level2(ctx);
	logger().debug("level1 after level2");
}

void start() {
	logging().debug("main before set ctx");
	Context ctx;
	auto logger = logging.withCtxFields(ctx);
	logger.setCtxField("main", "val");
	logger().debug("main before level1");
	level1(ctx);
	logger().debug("main after level1");
}

// TODO: test same key
int main() {
	auto cfg = ctx_log::Config{};
	ctx_log::setLogger(ctx_log::Config{
//						.JSON = true,
						.level = ctx_log::Level::DEBUG,
						.staticFields = ctx_log::CtxFields{{"key", "val"}},
						});

	start();
}
