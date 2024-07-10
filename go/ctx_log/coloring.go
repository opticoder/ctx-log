package ctx_log

import (
	"github.com/fatih/color"
	"go.uber.org/zap"
	"go.uber.org/zap/zapcore"
)

type levelColor struct {
	icon       string
	applyColor func(string, ...interface{}) string
}

var levelColorMap = map[zapcore.Level]levelColor{
	zapTraceLevel:  {"\U0001F6E0 ", color.HiCyanString},
	zap.DebugLevel: {"\U0001F4C3", color.HiBlueString},
	zap.InfoLevel:  {"\u2139\uFE0F ", color.HiWhiteString},
	zap.WarnLevel:  {"\u26A0\uFE0F ", color.HiYellowString},
	zap.ErrorLevel: {"\u274C", color.HiRedString},
	zap.PanicLevel: {"\u274C", color.New(color.FgWhite, color.BgRed).SprintfFunc()},
	zap.FatalLevel: {"\u2620\uFE0F ", color.New(color.FgWhite, color.BgHiRed).SprintfFunc()},
}

var applyModuleColor = color.CyanString
var applyCallerColor = color.HiBlackString
