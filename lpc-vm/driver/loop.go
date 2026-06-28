// driver/loop.go
package driver

import (
	"time"

	"mudscript/lpc-vm/object"
)

type ScheduledCall struct {
	Caller   *object.LPCObject
	FuncName string
	Closure  *object.Closure // 🚀 新增：支援直接呼叫閉包
	Args     []object.Object
	FireTime time.Time
}

func (d *Driver) runGameLoop() {
	ticker := time.NewTicker(d.Config.HeartBeatTick)
	defer ticker.Stop()
	for {
		select {
		case <-ticker.C:
			d.processHeartBeats()
			d.processCallOuts()
		case <-d.shutdownCh:
			return
		}
	}
}

func (d *Driver) processHeartBeats() {
	d.mu.RLock()
	targets := make([]*object.LPCObject, 0, len(d.Heartbeats))
	for obj := range d.Heartbeats {
		if obj != nil && !obj.IsDestructed {
			targets = append(targets, obj)
		}
	}
	d.mu.RUnlock()

	for _, obj := range targets {
		if obj.IsDestructed {
			continue
		}
		if conn := d.GetConnectionFromObject(obj); conn != nil {
			gid := getGID()
			d.playerContexts.Store(gid, conn)
			d.CallFunction(obj, "heart_beat", nil)
			d.playerContexts.Delete(gid)
		} else {
			d.CallFunction(obj, "heart_beat", nil)
		}
	}
}

func (d *Driver) CallOut(caller *object.LPCObject, funcName string, delay time.Duration, args ...object.Object) {
	fireTime := time.Now().Add(delay)
	d.mu.Lock()
	defer d.mu.Unlock()
	d.CallOuts = append(d.CallOuts, &ScheduledCall{
		Caller: caller, FuncName: funcName, Args: args, FireTime: fireTime,
	})
}

// 🚀 新增：支援閉包排程
func (d *Driver) CallOutClosure(caller *object.LPCObject, closure *object.Closure, delay time.Duration, args ...object.Object) {
	fireTime := time.Now().Add(delay)
	d.mu.Lock()
	defer d.mu.Unlock()
	d.CallOuts = append(d.CallOuts, &ScheduledCall{
		Caller: caller, Closure: closure, Args: args, FireTime: fireTime,
	})
}

func (d *Driver) processCallOuts() {
	now := time.Now()
	d.mu.Lock()
	var pending, ready []*ScheduledCall
	for _, call := range d.CallOuts {
		if now.After(call.FireTime) || now.Equal(call.FireTime) {
			ready = append(ready, call)
		} else {
			pending = append(pending, call)
		}
	}
	d.CallOuts = pending
	d.mu.Unlock()
	for _, call := range ready {
		if conn := d.GetConnectionFromObject(call.Caller); conn != nil {
			gid := getGID()
			d.playerContexts.Store(gid, conn)
			if call.Closure != nil {
				d.ExecuteCallback(call.Caller, call.Closure, call.Args)
			} else {
				d.CallFunction(call.Caller, call.FuncName, call.Args)
			}
			d.playerContexts.Delete(gid)
		} else {
			if call.Closure != nil {
				d.ExecuteCallback(call.Caller, call.Closure, call.Args)
			} else {
				d.CallFunction(call.Caller, call.FuncName, call.Args)
			}
		}
	}
}

func (d *Driver) SetHeartBeat(obj *object.LPCObject, enable bool) {
	if obj == nil || obj.IsDestructed {
		return
	}
	d.mu.Lock()
	defer d.mu.Unlock()
	if enable {
		d.Heartbeats[obj] = true
	} else {
		delete(d.Heartbeats, obj)
	}
}

func (d *Driver) runCleanUpLoop() {
	ticker := time.NewTicker(d.Config.CleanUpInterval)
	defer ticker.Stop()
	for {
		select {
		case <-ticker.C:
			d.processCleanUp()
		case <-d.shutdownCh:
			return
		}
	}
}

func (d *Driver) processCleanUp() {
	d.mu.Lock()
	objs := make([]*object.LPCObject, 0, len(d.ObjectTable))
	for _, obj := range d.ObjectTable {
		if obj != nil && !obj.IsDestructed {
			objs = append(objs, obj)
		}
	}
	d.mu.Unlock()

	now := time.Now().Unix()
	for _, obj := range objs {
		// 如果是 Master Object 或正在線上的玩家，不清理
		if obj == d.MasterObject || obj.IsInteractive {
			continue
		}

		// 呼叫 LPC 層級的 clean_up(inherited_count)
		// 這裡傳入 0 代表目前不追蹤繼承計數 (或是可以簡單實作)
		d.CallFunction(obj, "clean_up", []object.Object{&object.Integer{Value: 0}})

		// 檢查是否最後活動時間過久 (預設 30 分鐘沒活動)
		// 這只是個保險，真正的清理邏輯應該在 LPC 的 clean_up 中決定是否 destruct
		if now-obj.LastActivity > 1800 {
			// 如果物件沒人(Inventory為空) 且是 Clone，則考慮主動回收
			// 這部分通常交給 LPC 的 clean_up 實作會更精確
		}
	}
}
