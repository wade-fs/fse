package object

import "sync"

// Environment associates values with variable names.
type Environment interface {
	Get(name string) (Object, bool)
	GetMust(name string) Object // 🚀 新增：取得變數，若不存在則拋出 Panic
	Set(name string, val Object) Object
	Assign(name string, val Object) bool
	GetAll() map[string]Object
}

type environment struct {
	mu    sync.RWMutex
	store map[string]Object
	outer Environment
}

func NewEnvironment() Environment {
	return &environment{
		store: make(map[string]Object),
		outer: nil,
	}
}

func (e *environment) Get(name string) (Object, bool) {
	e.mu.RLock()
	obj, exists := e.store[name]
	e.mu.RUnlock()

	if !exists && e.outer != nil {
		obj, exists = e.outer.Get(name)
	}
	return obj, exists
}

func (e *environment) GetMust(name string) Object {
	obj, ok := e.Get(name)
	if !ok {
		panic("系統錯誤：找不到必要的環境變數: " + name)
	}
	return obj
}

func (e *environment) Set(name string, val Object) Object {
	e.mu.Lock()
	e.store[name] = val
	e.mu.Unlock()
	return val
}

func NewEnclosedEnvironment(outer Environment) Environment {
	return &environment{
		store: make(map[string]Object),
		outer: outer,
	}
}

func (e *environment) Assign(name string, val Object) bool {
	e.mu.Lock()
	if _, ok := e.store[name]; ok {
		e.store[name] = val
		e.mu.Unlock()
		return true
	}
	e.mu.Unlock()

	if e.outer != nil {
		if e.outer.Assign(name, val) {
			return true
		}
	}
	
	// 如果都沒找到，自動進行隱式宣告
	e.Set(name, val)
	return true
}

func (e *environment) GetAll() map[string]Object {
	e.mu.RLock()
	defer e.mu.RUnlock()
	result := make(map[string]Object)
	for k, v := range e.store {
		result[k] = v
	}
	return result
}
