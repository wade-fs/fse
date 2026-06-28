// driver/stack.go
package driver

import (
	"fmt"
	"strings"

	"mudscript/lpc-vm/object"
)

type RuntimeError struct {
	Message  string
	File     string
	Function string
	Stack    []string // 呼叫堆疊
}

type callFrame struct {
	File     string
	Function string
	Object   *object.LPCObject // 🚀 新增：執行此框架的物件
	Origin   int               // 🚀 新增：呼叫來源類型
}

func (d *Driver) getCallStack() []callFrame {
	gid := getGID()
	if val, ok := d.callStacks.Load(gid); ok {
		return val.([]callFrame)
	}
	return nil
}

func (d *Driver) pushFrame(f callFrame) {
	gid := getGID()
	stack := d.getCallStack()
	d.callStacks.Store(gid, append(stack, f))
}

func (d *Driver) popFrame() {
	gid := getGID()
	stack := d.getCallStack()
	if len(stack) > 0 {
		d.callStacks.Store(gid, stack[:len(stack)-1])
	} else {
		d.callStacks.Delete(gid)
	}
}

func (d *Driver) buildCallStack() []string {
	stack := d.getCallStack()
	var s []string
	for _, f := range stack {
		s = append(s, fmt.Sprintf("%s::%s()", f.File, f.Function))
	}
	return s
}

func (d *Driver) GetPreviousObject() *object.LPCObject {
	stack := d.getCallStack()
	if len(stack) < 2 {
		return nil
	}

	// 堆疊頂端是目前正在執行的 (this_object)
	// 前一個就是 previous_object
	// 但要排除連續在同一個物件內的函式呼叫
	currentObj := stack[len(stack)-1].Object
	for i := len(stack) - 2; i >= 0; i-- {
		if stack[i].Object != currentObj {
			return stack[i].Object
		}
	}
	return nil
}

func (e *RuntimeError) Error() string {
	var sb strings.Builder
	sb.WriteString(fmt.Sprintf("🔥 Runtime Error in %s::%s()\n", e.File, e.Function))
	sb.WriteString(fmt.Sprintf("   %s\n\n", e.Message))

	if len(e.Stack) > 0 {
		sb.WriteString("Call Stack:\n")
		for i, frame := range e.Stack {
			sb.WriteString(fmt.Sprintf("  %2d. %s\n", len(e.Stack)-i, frame))
		}
	}
	return sb.String()
}
