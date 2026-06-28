// driver/execute.go
package driver

import (
	"fmt"
	"log"
	"time"

	"mudscript/lpc-vm/evaluator"
	"mudscript/lpc-vm/object"
)

func (d *Driver) GetThisObject() *object.LPCObject {
	if p := d.GetCurrentPlayer(); p != nil && p.Object != nil {
		return p.Object
	}
	return nil
}

// 🚀 新增：暫時設定玩家上下文，回傳恢復函式
func (d *Driver) setPlayerContext(p *PlayerConnection) func() {
	gid := getGID()
	old, ok := d.playerContexts.Load(gid)
	d.playerContexts.Store(gid, p)
	return func() {
		if ok {
			d.playerContexts.Store(gid, old)
		} else {
			d.playerContexts.Delete(gid)
		}
	}
}

// 取得當前 Goroutine 對應的玩家
func (d *Driver) GetCurrentPlayer() *PlayerConnection {
	if p, ok := d.playerContexts.Load(getGID()); ok {
		return p.(*PlayerConnection)
	}
	return nil
}

// 專門給玩家網路層呼叫的進入點 (自動綁定上下文)
func (d *Driver) RunCommand(p *PlayerConnection, obj *object.LPCObject, funcName string, args []object.Object) object.Object {
	gid := getGID()
	oldContext, hasOld := d.playerContexts.Load(gid)
	if p != nil {
		d.playerContexts.Store(gid, p)
	}
	defer func() {
		if hasOld {
			d.playerContexts.Store(gid, oldContext)
		} else {
			d.playerContexts.Delete(gid)
		}
	}()
	return d.CallFunction(obj, funcName, args)
}

func (d *Driver) CallFunction(obj *object.LPCObject, funcName string, args []object.Object) object.Object {
	if obj == nil || obj.IsDestructed {
		return &object.Integer{Value: 0}
	}

	// 🚀 更新活動時間
	obj.LastActivity = time.Now().Unix()

	frame := callFrame{File: obj.Filename, Function: funcName, Object: obj}

	d.pushFrame(frame)
	defer d.popFrame()

	if obj.Vars != nil {
		obj.Vars.Set("this_object", &object.Builtin{
			Fn: func(args ...object.Object) object.Object { return obj },
		})
	}
	fnObj, ok := obj.Vars.Get(funcName)
	if !ok {
		// 🚀 關鍵：如果當前物件找不到函式，且有設定 SimulEfun 物件，則去那裡找

		if d.SimulEfunObj != nil && obj != d.SimulEfunObj {
			return d.CallFunction(d.SimulEfunObj, funcName, args)
		}
		return nil
	}
	if builtin, ok := fnObj.(*object.Builtin); ok {
		return builtin.Fn(args...)
	}
	fn, ok := fnObj.(*object.Function)
	if !ok {
		return object.NewError("%s is not a function", funcName)
	}

	extendedEnv := object.NewEnclosedEnvironment(obj.Vars)
	extendedEnv.Set("__origin_file", &object.String{Value: fn.OriginFile})
	if d.SimulEfunObj != nil {
		extendedEnv.Set("__simul_efun_obj", d.SimulEfunObj)
	}

	for i, param := range fn.Parameters {
		if i < len(args) {
			extendedEnv.Set(param.Value, args[i])
		} else {
			extendedEnv.Set(param.Value, &object.Nil{})
		}
	}
	evaluated := evaluator.Eval(fn.Body, extendedEnv)
	if errObj, ok := evaluated.(*object.Error); ok {
		runtimeErr := &RuntimeError{
			Message:  errObj.Message,
			File:     obj.Filename,
			Function: funcName,
			Stack:    d.buildCallStack(),
		}
		log.Printf("🔥 RUNTIME ERROR: %s", runtimeErr.Error())
		if d.MasterObject != nil && obj != d.MasterObject {
			d.CallFunction(d.MasterObject, "runtime_error", []object.Object{
				&object.String{Value: runtimeErr.Error()},
				&object.String{Value: obj.Filename},
			})
		} else {
			fmt.Println(runtimeErr.Error())
		}
		return nil
	}
	if returnValue, ok := evaluated.(*object.ReturnValue); ok {
		return returnValue.Value
	}
	return evaluated
}

func (d *Driver) AcceptConnection(pConn *PlayerConnection, arg interface{}) *object.LPCObject {
	if d.MasterObject == nil {
		return nil
	}
	gid := getGID()
	d.playerContexts.Store(gid, pConn)
	defer d.playerContexts.Delete(gid)

	var lpcArg object.Object
	switch v := arg.(type) {
	case int:
		lpcArg = &object.Integer{Value: int64(v)}
	case string:
		lpcArg = &object.String{Value: v}
	default:
		lpcArg = &object.Nil{}
	}

	args := []object.Object{lpcArg}
	result := d.CallFunction(d.MasterObject, "connect", args)
	if loginObj, ok := result.(*object.LPCObject); ok {
		return loginObj
	}
	return nil
}

func (d *Driver) TransferConnection(target *object.LPCObject, src *object.LPCObject) bool {
	if target == nil || src == nil {
		return false
	}
	d.mu.Lock()
	defer d.mu.Unlock()
	var connToMove *PlayerConnection
	if conn, ok := d.interactiveObjects.Load(src.Filename); ok {
		connToMove = conn.(*PlayerConnection)
	} else {
		d.interactiveObjects.Range(func(key, value interface{}) bool {
			if pconn, ok := value.(*PlayerConnection); ok && pconn.Object == src {
				connToMove = pconn
				return false
			}
			return true
		})
	}
	if connToMove == nil {
		return false
	}
	d.interactiveObjects.Delete(src.Filename)
	src.IsInteractive = false
	connToMove.Object = target
	d.interactiveObjects.Store(target.Filename, connToMove)
	target.IsInteractive = true
	gid := getGID()
	d.playerContexts.Store(gid, connToMove)
	return true
}
