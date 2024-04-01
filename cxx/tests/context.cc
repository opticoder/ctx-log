#include <ctx-log/ctx-log.h>

#ifdef CONTEXT_ENGINE_USERVER
#include "userver_run.h"
#endif

auto logging = ctx_log::getLogger("module1");


void level3() {
	auto logger = logging.withCtxFields();
	logger().debug("level3 before set ctx");
	logger.setCtxField("level3", "v");
	logger().debug("level3 after set ctx");
}

void level2() {
	auto logger = logging.withCtxFields();
	logger().debug("level2 before set ctx");
	logger.setCtxField("level2", "v");
	logger().debug("level2 after set ctx");
#ifdef CONTEXT_ENGINE_USERVER
	userver::utils::Async("level3", level3).Get();
#else
	level3();
#endif
}

void level1() {
	auto logger = logging.withCtxFields();
	logger().debug("level1 before set ctx");
	logger.setCtxField("level1", "v");
	logger().debug("level1 after set ctx");
	level2();
	logger().debug("level1 after level2");
}

void start() {
	logging().debug("main before set ctx");
	auto logger = logging.withCtxFields();
	logger.setCtxField("main", "v");
	logger().debug("main before level1");
	level1();
	logger().debug("main after level1");
}

// TODO: test same key
int main() {
	auto cfg = ctx_log::Config{};
	ctx_log::setLogger(ctx_log::Config{
//						.JSON = true,
						.level = ctx_log::Level::DEBUG,
						.staticFields = ctx_log::CtxFields{{"key", "val"}},
#ifdef CONTEXT_ENGINE_USERVER
						.context = ctx_log::InitUserverContext(),
#endif
						});

#ifdef CONTEXT_ENGINE_USERVER
	auto logger = ctx_log::getLogger("main", ctx_log::InitThreadsContext());
	logger().debug("engine: userver");
	run_userver();
#else
	logging().debug("engine: threads");
	start();
#endif
}
