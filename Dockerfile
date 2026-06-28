# 1. Build Stage
FROM golang:1.26.2-alpine AS builder

WORKDIR /app

# 安裝編譯與測試所需的工具，包含 GCC、musl-dev 等
RUN apk add --no-cache make git gcc musl-dev python3

# 複製依賴文件
COPY go.mod go.sum ./
RUN go mod download

# 複製源碼
COPY . .

# 編譯 Linux 版本
RUN make fsmud

# 2. Final Stage
FROM alpine:latest

WORKDIR /app

# 安裝運行時必要的套件，包含 python3 以支援 new-site 等腳本
RUN apk add --no-cache libc6-compat ca-certificates python3

# 從 Build Stage 複製編譯好的執行檔與必要資源
COPY --from=builder /app/bin/fsmud /app/bin/fsmud
COPY --from=builder /app/bin/new-site /app/bin/new-site
COPY --from=builder /app/mudlib /app/mudlib
COPY --from=builder /app/testlib /app/testlib


# 預設監聽 8080 連接埠
EXPOSE 8080

# 啟動指令：執行 MUD 伺服器
CMD ["./bin/fsmud", "-mudlib", "mudlib", "-master", "mudlib/master.c"]

