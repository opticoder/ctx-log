WIP: C++20 implementation of ctx-log logger

demo.cc
```c++
#include "ctx-log.h"

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

void start() {
	logger().trace("main before", "func1");
	func1();
	logger().fatal("main", "after", "func1");
}

int main() {
	ctx_log::setLogger(ctx_log::Config{
//						.JSON = true,
						.level = ctx_log::Level::TRACE,
						.staticFields = {{"key", "val"}},
						});
	start();
}
```

##### Console
![](assets/console.png)

##### JSON
```
{"time":"2024-03-01T00:11:22.333Z","level":"trace","module":"module1","caller":"tests/demo.cc:30","msg":"main before func1","key":"val"}
{"time":"2024-03-01T00:11:22.333Z","level":"debug","module":"module1","caller":"tests/demo.cc:16","msg":"func1 before set ctx","key":"val"}
{"time":"2024-03-01T00:11:22.333Z","level":"info","module":"module1","caller":"tests/demo.cc:18","msg":"func1 after set ctx","key":"val","func1":"val"}
{"time":"2024-03-01T00:11:22.333Z","level":"warn","module":"module1","caller":"tests/demo.cc:9","msg":"func2 before set ctx","key":"val","func1":"val"}
{"time":"2024-03-01T00:11:22.333Z","level":"error","module":"module1","caller":"tests/demo.cc:11","msg":"func2 after set ctx","key":"val","func1":"val","func2":"val"}
{"time":"2024-03-01T00:11:22.333Z","level":"crit","module":"module1","caller":"tests/demo.cc:20","msg":"func1 after func2","func1":"val","key":"val"}
{"time":"2024-03-01T00:11:22.333Z","level":"fatal","module":"module1","caller":"tests/demo.cc:32","msg":"main after func1","key":"val"}
```

### Build and run
With threads context engine
```bash
cd cxx
conan create . --build=missing --test-folder tests
```

With [userver](https://userver.tech) context engine
```bash
cd cxx
conan create . --build=missing --test-folder tests -o context_engine=userver
```

### TODO
- [ ] explicit context (Golang-style) handling
- [ ] add others' async frameworks context engines
- [ ] console icons/colors disabling ability
- [ ] fields renaming/hiding
- [ ] output tests with pytest
- [ ] other modules logs intercepting
