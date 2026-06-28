package evaluator

import (
	"mudscript/lpc-vm/object"
)

var builtins = map[string]*object.Builtin{
	// LPC scripts use Efuns provided by the driver. 
	// This map can be empty or only contain internal engine built-ins if needed.
}

func RegisterBuiltin(name string, builtin *object.Builtin) {
	builtins[name] = builtin
}
