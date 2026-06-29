# adventures/pc/Makefile
# FSE PC (Prehistoric Civilization / 史前文明) 啟動配置

TOP=$(shell pwd)
OUT=$(TOP)/../../bin

.PHONY: all run-pc test-pc clean

all:
	@echo "ℹ️  PC 史前文明 使用共用 FSE 引擎，無需本地編譯。"

run-pc:
	@echo "🦕 啟動 FSE: PC — 史前文明 (Prehistoric Civilization) on port 9091..."
	@$(OUT)/fsmud -mudlib mudlib -master master.c --hub none -port 9091

test-pc:
	@echo "🧪 運行 PC 最小啟動測試..."
	@MUD_TEST_MODE=1 $(OUT)/fsmud -mudlib mudlib -master master.c --hub none 2>&1

clean:
	@rm -f mudlib/data/state/players/*.o
	@rm -f mudlib/data/state/system/*.o
