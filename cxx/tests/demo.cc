#include <ctx-log/ctx-log.h>


auto logging = ctx_log::getLogger("module1");


void func2() {
	auto logger = logging.withCtxFields();
	logger().warnf("%s before set ctx", "func2");
	logger.setCtxField("func2", "val");
	logger().errorf("%s after set ctx", "func2");
}

void func1() {
	auto logger = logging.withCtxFields();
	logger().debugf("%s before set ctx", "func1");
	logger.setCtxField("func1", "val");
	logger().infof("%s after set ctx", "func1");
	func2();
	logger().critf("%s after func2", "func1");
}

void start() {
	logging().trace("main before", "func1");
	func1();
	logging().fatal("main", "after", "func1");
}

int main() {
	ctx_log::setLogger(ctx_log::Config{
//						.JSON = true,
						.level = ctx_log::Level::TRACE,
						.staticFields = {{"key", "val"}},
						});
	start();
}
