// driver/utils.go
package driver

import (
	"bytes"
	"fmt"
	"runtime"
	"strconv"

	"mudscript/lpc-vm/object"
)

// 取得當前 Goroutine ID
func getGID() uint64 {
	b := make([]byte, 64)
	b = b[:runtime.Stack(b, false)]
	b = bytes.TrimPrefix(b, []byte("goroutine "))
	b = b[:bytes.IndexByte(b, ' ')]
	n, _ := strconv.ParseUint(string(b), 10, 64)
	return n
}

var cloneCounter int

func generateCloneID() string {
	cloneCounter++
	return fmt.Sprintf("%d", cloneCounter)
}

func deepCopyLPCValue(obj object.Object) object.Object {
	if obj == nil {
		return nil
	}
	switch o := obj.(type) {
	case *object.Array:
		newElems := make([]object.Object, len(o.Elements))
		for i, el := range o.Elements {
			newElems[i] = deepCopyLPCValue(el)
		}
		return &object.Array{Elements: newElems}
	case *object.Mapping:
		newPairs := make(map[object.HashKey]object.HashPair)
		for k, v := range o.Pairs {
			newPairs[k] = object.HashPair{
				Key:   deepCopyLPCValue(v.Key),
				Value: deepCopyLPCValue(v.Value),
			}
		}
		return &object.Mapping{Pairs: newPairs}
	default:
		return obj
	}
}
