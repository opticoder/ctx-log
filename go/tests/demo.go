package main

import (
	"context"

	"github.com/opticoder/ctx-log/go/ctx_log"
)

var logging = ctx_log.GetLogger("module1")

func func2(ctx context.Context) {
	logger := logging.WithCtxFields(ctx)
	logger.Warnf("%s before set ctx", "func2")
	logger.SetCtxField(ctx, "func2", "val")
	logger.Errorf("%s after set ctx", "func2")
}

func func1(ctx context.Context) {
	defer func() {
		recover()
	}()
	logger := logging.WithCtxFields(ctx)
	logger.Debugf("%s before set ctx", "func1")
	ctx = logger.SetCtxField(ctx, "func1", "val")
	logger.Infof("%s after set ctx", "func1")
	func2(ctx)
	logger.Panicf("%s after func2", "func1")
}

func start() {
	logging.Trace("main before", "func1")
	func1(context.Background())
	logging.Fatal("main", "after", "func1")
}

func main() {
	err := ctx_log.SetLogger(&ctx_log.Config{
		//JSON:         true,
		Level:        ctx_log.TraceLevel,
		StaticFields: map[string]string{"key": "val"},
	})
	if err != nil {
		panic(err)
	}
	start()
}
