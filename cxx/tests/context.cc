#include "ctx-log.h"

#ifdef CONTEXT_ENGINE_USERVER
#include "userver_run.h"
#endif

auto logger = ctx_log::getLogger("handlers");


void level3() {
	auto ctx = logger.withCtxFields();
	logger().debug("level3 before set ctx");
	logger.setCtxField(ctx, "level3", "v");
	logger().debug("level3 after set ctx");
}

void level2() {
	auto ctx = logger.withCtxFields();
	logger().debug("level2 before set ctx");
	logger.setCtxField(ctx, "level2", "v");
	logger().debug("level2 after set ctx");
#ifdef CONTEXT_ENGINE_USERVER
	userver::utils::Async("level3", level3).Get();
#else
	level3();
#endif
}

void level1() {
	auto ctx = logger.withCtxFields();
	logger().debug("level1 before set ctx");
	logger.setCtxField(ctx, "level1", "v");
	logger().debug("level1 after set ctx");
	level2();
	logger().debug("level1 after level2");
}

void start(bool engine_threads) {
	auto ctx = logger.withCtxFields();
	if (engine_threads) logger().debug("engine: threads"); else logger().debug("engine: userver");
	logger.setCtxField(ctx, "level0", "v");
	level1();
	logger().debug("level0 after level1");
}

// TODO: test same key
int main() {
	auto cfg = ctx_log::Config{};
	ctx_log::setLogger(ctx_log::Config{
						.JSON = true,
						.level = ctx_log::Level::DEBUG,
						.staticFields = ctx_log::CtxFields{{"key", "val"}},
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
