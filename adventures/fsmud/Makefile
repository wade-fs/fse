#backend
DATE   := $(shell date +%Y%m%d)
USER := $(shell whoami)

COMMIT := $(shell git rev-parse --short=8 HEAD)
VERSION := $(COMMIT)-$(DATE)

TOP=$(shell pwd)
OUT=$(TOP)/bin

# 🚀 智慧路徑偵測：優先使用系統環境，找不到才用預設值
DETECTED_GOROOT := $(shell go env GOROOT 2>/dev/null)
GOROOT ?= $(DETECTED_GOROOT)
ifeq ($(GOROOT),)
    GOROOT := /usr/local/go-1.26.2
endif

# 🚀 根據 GOROOT 是否存在來決定 Go 執行檔路徑
GO_EXE := $(shell if [ -f $(GOROOT)/bin/go ]; then echo $(GOROOT)/bin/go; else echo go; fi)

# 環境變數配置 (移除強制 GOROOT 以相容 CI)
GO_FLAGS := -ldflags="-s -w"
COMMON_ENV := CGO_CFLAGS="-Wno-return-local-addr"
ENVW := $(COMMON_ENV) CGO_ENABLED=1 GOOS=windows GOARCH=amd64 CC="x86_64-w64-mingw32-gcc -fno-stack-protector -D_FORTIFY_SOURCE=0 -lssp"

.PHONY: all clean test test-driver test-fsmud fsmud fsmud.exe run-fsmud audit-settlement audit-settlement-all lint-yaml

all: fsmud fsmud.exe

clean-txt:
	@ rm -f *txt *log

$(OUT):
	@mkdir -p $(OUT)

# 編譯 Linux 版本
fsmud: $(OUT)
	@echo "🔨 Building $@ (Linux Standard)..."
	@cd ../.. && go mod tidy && $(COMMON_ENV) $(GO_EXE) build $(GO_FLAGS) -tags fsmud -o adventures/fsmud/bin/$@ ./adventures/cmd/fse
	@ls -l $(OUT)/$@

# 編譯 Windows 版本 (.exe)
fsmud.exe: $(OUT)
	@echo "🔨 Building $@ (Windows Standard)..."
	@cd ../.. && go mod tidy && $(ENVW) $(GO_EXE) build $(GO_FLAGS) -tags fsmud -o adventures/fsmud/bin/$@ ./adventures/cmd/fse
	@ls -l $(OUT)/$@

mudscript: $(OUT)
	@echo "🔨 Building $@ (Linux Pure)..."
	@cd ../.. && go mod tidy && $(COMMON_ENV) $(GO_EXE) build $(GO_FLAGS) -o adventures/fsmud/bin/$@ ./adventures/cmd/fse
	@ls -l $(OUT)/$@

# 執行測試
test-driver: fsmud
	@echo "🧪 Running MudScript Core Tests on driver in isolation..."
	@MUD_TEST_MODE=1 $(OUT)/fsmud -mudlib testlib -master master.c --hub none 2>&1 | tee test-driver.txt

test: test-driver

# 正常執行伺服器
run run-fsmud: fsmud
	@echo "🚀 Starting MudScript Server (Connecting to Global Hub) on port 8080..."
	@ ./bin/fsmud -mudlib mudlib -master master.c -port 8080 2>&1 | tee run-fsmud.txt

# 執行 FSMUD 自動化一鍵整合測試
test-fsmud: fsmud
	@echo "🧪 Running Integration Test on FSMUD..."
	@cd ../.. && go run ./adventures/fsmud/cmd/test-fsmud/main.go

# 🐳 Docker 執行與測試規則
.PHONY: docker-build docker-run docker-test-driver docker-test-fsmud

docker-build:
	@echo "🐳 Building Docker Image..."
	@docker build -t formosasaga:latest .

docker-run:
	@echo "🐳 Running MudScript Server in Docker container..."
	@docker-compose up --build

docker-test-driver:
	@echo "🐳 Running MudScript Core Tests in Docker container..."
	@docker run --rm -it -e MUD_TEST_MODE=1 formosasaga:latest ./bin/fsmud -mudlib testlib -master master.c --hub none


docker-test-fsmud:
	@echo "🐳 Running Integration Tests on FSMUD in Docker container..."
	@docker run --rm -it -v $(shell pwd):/app -w /app golang:1.26.2-alpine sh -c "apk add --no-cache make gcc musl-dev python3 && make test-fsmud"


clean:
	@rm -rf *.log *txt $(OUT)/*

# ── P23.1 聚落驗收稽核 ───────────────────────────────────────────────────────
# 用法：
#   make audit-settlement SETTLEMENT=minxiong   # 稽核單一聚落（預設 minxiong）
#   make audit-settlement-all                   # 稽核所有聚落
SETTLEMENT ?= minxiong

audit-settlement:
	@echo "📋 稽核聚落：$(SETTLEMENT)"
	@python3 check_settlement.py $(SETTLEMENT)

audit-settlement-all:
	@echo "📋 稽核所有聚落..."
	@python3 check_settlement.py --all

lint-yaml:
	@echo "🔍 Lint YAML files..."
	@python3 scripts/lint_yaml.py
