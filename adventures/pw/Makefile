# adventures/pw/Makefile
# Standard FSE compile and launch configurations for PW (Programming World)

TOP=$(shell pwd)
OUT=$(TOP)/../../bin

.PHONY: all run-pw test-pw clean

all:
	@echo "ℹ️ PW campaign uses the shared FSE engine. No local compilation needed."

run-pw:
	@echo "🚀 Starting FSE: PW (Programming World) on port 9090..."
	@$(OUT)/fsmud -mudlib mudlib -master master.c --hub none -port 9090

test-pw:
	@echo "🧪 Running PW Minimal Closed Loop Tests..."
	@MUD_TEST_MODE=1 $(OUT)/fsmud -mudlib mudlib -master master.c --hub none 2>&1
