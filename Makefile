# Flow State Engine (FSE) Master Makefile
# Configuration for PW (Programming World) and core FSE compilation

DATE   := $(shell date +%Y%m%d)
COMMIT := $(shell git rev-parse --short=8 HEAD 2>/dev/null || echo "unknown")
VERSION := $(COMMIT)-$(DATE)

TOP=$(shell pwd)
OUT=$(TOP)/bin

DETECTED_GOROOT := $(shell go env GOROOT 2>/dev/null)
GOROOT ?= $(DETECTED_GOROOT)
ifeq ($(GOROOT),)
    GOROOT := /usr/local/go-1.26.2
endif

GO_EXE := $(shell if [ -f $(GOROOT)/bin/go ]; then echo $(GOROOT)/bin/go; else echo go; fi)

GO_FLAGS := -ldflags="-s -w"
COMMON_ENV := CGO_CFLAGS="-Wno-return-local-addr"

.PHONY: all clean run-pw test-pw run-pc test-pc build-engine

all: build-engine

$(OUT):
	@mkdir -p $(OUT)

build-engine: $(OUT)
	@echo "🔨 Building fsmud engine (Linux)..."
	@go mod tidy
	@$(COMMON_ENV) $(GO_EXE) build $(GO_FLAGS) -o $(OUT)/fsmud ./adventures/cmd/fse
	@ls -l $(OUT)/fsmud

clean:
	@rm -rf $(OUT)
	@rm -f adventures/pw/mudlib/data/state/system/*.o

run-pw: build-engine
	@make -C adventures/pw run-pw

test-pw: build-engine
	@make -C adventures/pw test-pw

run-pc: build-engine
	@make -C adventures/pc run-pc

test-pc: build-engine
	@make -C adventures/pc test-pc
