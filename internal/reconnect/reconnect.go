package reconnect

import "time"

func Retry(fn func() error) {

	for {

		err := fn()
		if err == nil {
			return
		}

		time.Sleep(2 * time.Second)
	}
}
