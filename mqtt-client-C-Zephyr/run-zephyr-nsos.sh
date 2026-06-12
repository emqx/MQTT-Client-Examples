#!/bin/bash

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
BUILD_DIR=${BUILD_DIR:-"$SCRIPT_DIR/build-nsos"}
APP_PATH=${APP_PATH:-"$BUILD_DIR/zephyr/zephyr.exe"}

if [[ ! -x "$APP_PATH" ]]; then
    echo "❌ 未找到 NSOS 可执行文件: $APP_PATH"
    echo "   请先执行:"
    echo "   west build -d build-nsos -p always -b native_sim/native/64 . -- -DOVERLAY_CONFIG=prj-nsos.conf"
    exit 1
fi

# echo "🧹 清理旧进程..."
# pkill -9 -f zephyr.exe 2>/dev/null || true

echo "🟢 启动 Zephyr (NSOS/TCP-only)..."
echo "   (输入命令到 uart:~$，Ctrl+C 退出)"
echo ""

exec "$APP_PATH"