#!/bin/bash
TAG=$1
[ -z "$TAG" ] && echo "Usage: $0 TAG (e.g., v1.1)" && exit 1

# 改善 Regex，支援 v1.1, v1.1.0 等格式
if ! [[ "$TAG" =~ ^v[0-9]+(\.[0-9]+)*$ ]]; then
    echo "錯誤格式。請使用 v#.# 格式 (例如 v1.1)"
    exit 1
fi

# 刪除本地舊 Tag (若存在)
git tag -d "$TAG" >/dev/null 2>&1 || true

# 刪除遠端舊 Tag，以便 GitHub Actions 重新觸發 (使用變數 $TAG 而非硬編碼)
git push origin :refs/tags/"$TAG" >/dev/null 2>&1 || true

# 建立並推送新 Tag
echo "正在推送 Tag: $TAG ..."
git tag "$TAG"
git push origin "$TAG" --force
