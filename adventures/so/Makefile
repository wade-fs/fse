# adventures/so/Makefile
# FSE SO (Shushan Odyssey / 蜀山遊記) 啟動與測試配置

TOP=$(shell pwd)
OUT=$(TOP)/../../bin

.PHONY: all run-so test-so clean

all:
	@echo "ℹ️  SO 蜀山遊記 使用共用 FSE 引擎，無需本地編譯。"

run-so:
	@echo "🔮 啟動 FSE: SO — 尋仙問道 on port 9092..."
	@$(OUT)/fsmud -mudlib mudlib -port 9092 --hub none

test-so:
	@echo "🧪 運行 SO 最小啟動測試..."
	@MUD_TEST_MODE=1 $(OUT)/fsmud -mudlib mudlib --hub none 2>&1

clean:
	@rm -f ../../data/so/state/players/*.o
	@rm -f ../../data/so/state/system/*.o
