package ctx_log

import (
	"context"
	"encoding/json"
	"errors"
	"fmt"
	"os"
	"strconv"
	"strings"

	"go.uber.org/zap"
	"go.uber.org/zap/buffer"
	"go.uber.org/zap/zapcore"
)

// fieldsKey struct{}
const ctxFieldsKey = "log"

type CtxFields []zapcore.Field

const zapTraceLevel = zapcore.DebugLevel - 1

type ctxLogger struct {
	loggerBase *zap.Logger
	name       string
	logger     *zap.Logger
	//level      Level
}

func getLogger(name string) ContextLogger {
	//logger := loggers[name]
	//if logger != nil {
	//	return logger
	//}
	// TODO: module field
	logger := &ctxLogger{zap.L(), name, zap.L().Named(name)}
	//logger := &ctxLogger{zap.L()}

	loggers[name] = logger //.logger.With(zap.String("module", name))
	return logger
}

func (l *ctxLogger) getLogger() *zap.Logger {
	if l.loggerBase != zap.L() { //.WithOptions() {
		l.loggerBase = zap.L()
		l.logger = zap.L().Named(l.name)
	}
	return l.logger
}

func text2Level(level string) Level {
	logLevel := FatalLevel + 1
	switch strings.ToUpper(level) {
	case "TRACE":
		logLevel = TraceLevel
	case "DEBUG":
		logLevel = DebugLevel
	case "INFO":
		logLevel = InfoLevel
	case "WARN":
		logLevel = WarnLevel
	case "ERROR":
		logLevel = ErrorLevel
	case "PANIC":
		logLevel = PanicLevel
	case "FATAL":
		logLevel = FatalLevel
	}
	return logLevel
}

func zapLevel(lvl Level) *zapcore.Level {
	var level zapcore.Level

	switch lvl {
	case TraceLevel:
		level = zapTraceLevel
	case DebugLevel:
		level = zap.DebugLevel
	case InfoLevel:
		level = zap.InfoLevel
	case WarnLevel:
		level = zap.WarnLevel
	case ErrorLevel:
		level = zap.ErrorLevel
	case FatalLevel:
		level = zap.FatalLevel
		// 		panic level?
	default:
		return nil
	}

	return &level
}

func setLogger(config *Config) error {
	level := zapLevel(config.Level)

	if level == nil {
		return errors.New("wrong logging level")
	}

	messageField := messageFieldDefault
	if config.MessageField != nil {
		messageField = *config.MessageField
	}
	levelField := levelFieldDefault
	if config.LevelField != nil {
		levelField = *config.LevelField
	}
	timeField := timeFieldDefault
	if config.TimeField != nil {
		timeField = *config.TimeField
	}
	callerField := callerFieldDefault
	if config.CallerField != nil {
		callerField = *config.CallerField
	}
	moduleField := moduleFieldDefault
	if config.ModuleField != nil {
		moduleField = *config.ModuleField
	}

	var enc zapcore.Encoder
	if config.JSON {
		enc = zapcore.NewJSONEncoder(zapcore.EncoderConfig{
			LevelKey:     levelField,
			TimeKey:      timeField,
			CallerKey:    callerField,
			MessageKey:   messageField,
			NameKey:      moduleField,
			EncodeLevel:  lowercaseLevelEncoder,
			EncodeTime:   zapcore.ISO8601TimeEncoder,
			EncodeCaller: callerEncoder,
		})
	} else {
		enc = newCustomConsoleEncoder(moduleField)
	}

	logger := zap.New(zapcore.NewCore(enc,
		zapcore.AddSync(os.Stdout),
		zap.NewAtomicLevelAt(*level)),
		//zapTraceLevel),
		zap.AddCaller(),
		zap.AddCallerSkip(1),
		zap.AddStacktrace(zapTraceLevel),
	)
	for name, val := range config.StaticFields {
		logger = logger.With(zap.String(name, val))
	}
	_, err := zap.RedirectStdLogAt(logger, zapcore.InfoLevel)
	if err != nil {
		return err
	}
	zap.ReplaceGlobals(logger)
	//Logger = &ctxLogger{logger}
	return nil
}

func init() {
	enc := zapcore.NewJSONEncoder(zap.NewProductionEncoderConfig())
	logger := zap.New(zapcore.NewCore(enc,
		zapcore.AddSync(os.Stderr),
		zap.NewAtomicLevelAt(zapTraceLevel)),
		zap.AddCaller(),
		zap.AddCallerSkip(1),
	)
	zap.ReplaceGlobals(logger.With(zap.String("logger", "unconfigured")))
	//Logger = &ctxLogger{logger.With(zap.String("logger", "unconfigured"))}
}

func lowercaseLevelEncoder(level zapcore.Level, enc zapcore.PrimitiveArrayEncoder) {
	if level == zapTraceLevel {
		enc.AppendString("trace")
		return
	}
	zapcore.LowercaseLevelEncoder(level, enc)
}

// TODO: package -> module
//
//	version -> caller
func omitExternalPackageVersion(caller zapcore.EntryCaller) *string {
	str := caller.String()
	idx := strings.Index(str, "@")
	if idx == -1 {
		return nil
	}
	// root package name with full path
	path := strings.SplitAfter(str[0:idx], "/")

	// cut version with trailing path to subpackage
	str = str[idx:]

	// trim version
	idx = strings.Index(str, "/")
	str = str[idx:]

	// trim filename
	idx = strings.LastIndex(str, "/")
	if idx == -1 {
		idx = 0
	}
	str = str[0:idx]

	// domain + publisher + package name + subpackage path
	for i := range path {
		if i > 2 {
			break
		}
		str = path[len(path)-1-i] + str
	}
	return &str
}

func callerEncoder(caller zapcore.EntryCaller, enc zapcore.PrimitiveArrayEncoder) {
	if str := omitExternalPackageVersion(caller); str != nil {
		enc.AppendString(*str)
		return
	}
	zapcore.ShortCallerEncoder(caller, enc)
}

func newCustomConsoleEncoder(moduleField string) zapcore.Encoder {
	pool := buffer.NewPool()
	return consoleEncoder{moduleField, zapcore.NewJSONEncoder(zapcore.EncoderConfig{}), &pool}
}

type consoleEncoder struct {
	moduleField string
	zapcore.Encoder
	pool *buffer.Pool
}

func (e consoleEncoder) Clone() zapcore.Encoder {
	return consoleEncoder{e.moduleField, e.Encoder.Clone(), e.pool}
}

func (e consoleEncoder) EncodeEntry(entry zapcore.Entry, extra []zapcore.Field) (*buffer.Buffer, error) {
	jsonFields, err := e.Encoder.EncodeEntry(entry, extra)
	if err != nil {
		return jsonFields, err
	}
	var ctxFields map[string]string
	err = json.Unmarshal(jsonFields.Bytes(), &ctxFields)
	if err != nil {
		return jsonFields, err
	}
	jsonFields.Free()

	buf := e.pool.Get()

	var level string
	if entry.Level == zapTraceLevel {
		level = "TRACE"
	} else {
		level = entry.Level.CapitalString()
	}
	buf.AppendString(levelColorMap[entry.Level].icon + " ")
	buf.AppendString(levelColorMap[entry.Level].applyColor(fmt.Sprintf("%-5s", level)) + " ")

	buf.AppendString(entry.Message)

	var caller string
	if str := omitExternalPackageVersion(entry.Caller); str != nil {
		caller = *str
	} else {
		caller = entry.Caller.TrimmedPath()
	}
	module := entry.LoggerName
	if module != "" {
		entry.LoggerName = ""
	} else {
		module = strings.Split(caller, "/")[0]
	}
	buf.AppendString(applyModuleColor(" [" + module + "]"))
	buf.AppendString(applyCallerColor(" [" + caller + "]"))

	for name, val := range ctxFields {
		buf.AppendString(fmt.Sprintf(" %s=%s", name, strconv.Quote(val)))
	}

	buf.AppendString("\n")
	return buf, nil
}

func (l *ctxLogger) Trace(args ...interface{}) {
	//if l.
	l.getLogger().Log(zapTraceLevel, fmt.Sprint(args...))
}

func (l *ctxLogger) Tracef(msg string, args ...interface{}) {
	l.getLogger().Log(zapTraceLevel, fmt.Sprintf(msg, args...))
}

func (l *ctxLogger) Debug(args ...interface{}) {
	l.getLogger().Log(zap.DebugLevel, fmt.Sprint(args...))
}

func (l *ctxLogger) Debugf(msg string, args ...interface{}) {
	l.getLogger().Log(zap.DebugLevel, fmt.Sprintf(msg, args...))
}

func (l *ctxLogger) Info(args ...interface{}) {
	l.getLogger().Log(zap.InfoLevel, fmt.Sprint(args...))
}

func (l *ctxLogger) Infof(msg string, args ...interface{}) {
	l.getLogger().Log(zap.InfoLevel, fmt.Sprintf(msg, args...))
}

func (l *ctxLogger) Warn(args ...interface{}) {
	l.getLogger().Log(zap.WarnLevel, fmt.Sprint(args...))
}

func (l *ctxLogger) Warnf(msg string, args ...interface{}) {
	l.getLogger().Log(zap.WarnLevel, fmt.Sprintf(msg, args...))
}

func (l *ctxLogger) Error(args ...interface{}) {
	l.getLogger().Log(zap.ErrorLevel, fmt.Sprint(args...))
}

func (l *ctxLogger) Errorf(msg string, args ...interface{}) {
	l.getLogger().Log(zap.ErrorLevel, fmt.Sprintf(msg, args...))
}

func (l *ctxLogger) Panic(args ...interface{}) {
	l.getLogger().Log(zap.PanicLevel, fmt.Sprint(args...))
}

func (l *ctxLogger) Panicf(msg string, args ...interface{}) {
	l.getLogger().Log(zap.PanicLevel, fmt.Sprintf(msg, args...))
}

func (l *ctxLogger) Fatal(args ...interface{}) {
	l.getLogger().Log(zap.FatalLevel, fmt.Sprint(args...))
}

func (l *ctxLogger) Fatalf(msg string, args ...interface{}) {
	l.getLogger().Log(zap.FatalLevel, fmt.Sprintf(msg, args...))
}

func (l *ctxLogger) IsEnabledFor(lvl Level) bool {
	level := zapLevel(lvl)
	if level == nil {
		return false
	}
	return l.getLogger().Level() <= *level
}

func (l *ctxLogger) WithCtxFields(ctx context.Context) ContextLogger {
	logger := &ctxLogger{l.loggerBase, l.name, l.getLogger()}
	if ctxFields, ok := ctx.Value(ctxFieldsKey).(CtxFields); ok {
		logger.logger = logger.logger.With(ctxFields...)
	}
	return logger
}

func (l *ctxLogger) SetCtxField(ctx context.Context, name string, val string) context.Context {
	field := zapcore.Field{Key: name, String: val, Type: zapcore.StringType}
	ctxFields, ok := ctx.Value(ctxFieldsKey).(CtxFields)
	if ok {
		ctxFields = append(ctxFields, field)
	} else {
		ctxFields = CtxFields{field}
	}
	l.logger = l.getLogger().With(zap.String(name, val))
	return context.WithValue(ctx, ctxFieldsKey, ctxFields)
}

func (l *ctxLogger) WithField(name string, val string) ContextLogger {
	return &ctxLogger{l.loggerBase, l.name, l.getLogger().With(zap.String(name, val))}
}

func (l *ctxLogger) WithError(err error) ContextLogger {
	return &ctxLogger{l.loggerBase, l.name, l.getLogger().With(zap.Error(err))}
}

func (l *ctxLogger) SkipCallers(count int) ContextLogger {
	return &ctxLogger{l.loggerBase, l.name, l.getLogger().WithOptions(zap.AddCallerSkip(count))}
}
