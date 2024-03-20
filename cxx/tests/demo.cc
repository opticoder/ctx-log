#include "ctx-log.h"
#ifdef CONTEXT_ENGINE_USERVER
#include "userver_run.h"
#endif

auto logger = ctx_log::getLogger("module1");

void func2() {
	auto ctx = logger.withCtxFields();
	logger().warnf("%s before set ctx", "func2");
	logger.setCtxField(ctx, "func2", "val");
	logger().errorf("%s after set ctx", "func2");
}

void func1() {
	auto ctx = logger.withCtxFields();
	logger().debugf("%s before set ctx", "func1");
	logger.setCtxField(ctx, "func1", "val");
	logger().infof("%s after set ctx", "func1");
	func2();
	logger().critf("%s after func2", "func1");
}

void start(bool engine_threads) {
	if (engine_threads) logger().debug("engine: threads"); else logger().debug("engine: userver");
	logger().trace("main before", "func1");
	func1();
	logger().fatal("main", "after", "func1");
}

int main() {
	ctx_log::setLogger(ctx_log::Config{
//						.JSON = true,
						.level = ctx_log::Level::TRACE,
						.staticFields = {{"key", "val"}},
#ifdef CONTEXT_ENGINE_USERVER
						.context = ctx_log::InitUserverContext(),
#endif
						});

#ifdef CONTEXT_ENGINE_USERVER
	run_userver();
#else
	start(true);
#endif
}
