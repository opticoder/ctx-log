package ctx_log

import (
	"context"
	"runtime"
	"strings"
)

type ContextLogger interface {
	WithCtxFields(ctx context.Context) ContextLogger
	SetCtxField(ctx context.Context, key string, value string) context.Context

	IsEnabledFor(level Level) bool
	// LogFor(level Level, fmt func())

	Trace(args ...interface{})
	Tracef(msg string, args ...interface{})
	Debug(args ...interface{})
	Debugf(msg string, args ...interface{})
	Info(args ...interface{})
	Infof(msg string, args ...interface{})
	Warn(args ...interface{})
	Warnf(msg string, args ...interface{})
	Error(args ...interface{})
	Errorf(msg string, args ...interface{})
	//      TODO: WithPanic
	Panic(args ...interface{})
	Panicf(msg string, args ...interface{})
	Fatal(args ...interface{})
	Fatalf(msg string, args ...interface{})

	WithField(key string, value string) ContextLogger
	WithError(err error) ContextLogger

	SkipCallers(count int) ContextLogger
}

var loggers = map[string]ContextLogger{}

//var Logger ContextLogger

type Level uint

const (
	TraceLevel Level = iota
	// DebugLevel level. Usually only enabled when debugging. Very verbose logging.
	DebugLevel
	// InfoLevel level. General operational entries about what's going on inside the
	// application.
	InfoLevel
	// WarnLevel level. Non-critical entries that deserve eyes.
	WarnLevel
	// ErrorLevel level. Logs. Used for errors that should definitely be noted.
	// Commonly used for hooks to send errors to an error tracking service.
	ErrorLevel
	// PanicLevel level, highest level of severity. Logs and then calls panic with the
	// message passed to Debug, Info, ...
	PanicLevel
	// FatalLevel level. Logs and then calls `os.Exit(1)`. It will exit even if the
	// logging level is set to Panic.
	FatalLevel
)

const messageFieldDefault string = "msg"
const levelFieldDefault string = "level"
const timeFieldDefault string = "time"
const callerFieldDefault string = "caller"
const moduleFieldDefault string = "module"

type Config struct {
	JSON         bool
	Level        Level
	StaticFields map[string]string

	MessageField *string
	LevelField   *string
	TimeField    *string
	CallerField  *string
	ModuleField  *string
}

func retrieveCallInfo() string {
	pc, _, _, _ := runtime.Caller(2)
	parts := strings.Split(runtime.FuncForPC(pc).Name(), ".")
	pl := len(parts)
	packageName := ""

	if parts[pl-2][0] == '(' {
		packageName = parts[pl-3]
	} else {
		packageName = parts[pl-2]
	}
	// trim filename
	idx := strings.LastIndex(packageName, "/")
	packageName = packageName[idx+1:]

	return packageName
}

// TODO: initLogging
func SetLogger(config *Config) error {
	return setLogger(config)
}

func GetLogger(module interface{}) ContextLogger {
	name, ok := module.(string)
	if ok {
		return getLogger(name)
	}
	return getLogger(retrieveCallInfo())
}

func Text2Level(level string) Level {
	return text2Level(level)
}
