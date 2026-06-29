#!/bin/bash

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
BUILD_DIR=${BUILD_DIR:-"$SCRIPT_DIR/build-nsos"}
APP_PATH=${APP_PATH:-"$BUILD_DIR/zephyr/zephyr.exe"}

if [[ ! -x "$APP_PATH" ]]; then
    echo "❌ NSOS executable not found: $APP_PATH"
    echo "   Run the build first:"
    echo "   west build -d build-nsos -p always -b native_sim/native/64 . -- -DOVERLAY_CONFIG=prj-nsos.conf"
    exit 1
fi

echo "🟢 Starting Zephyr (NSOS/TCP-only)..."
echo "   (Type commands at uart:~$, Ctrl+C to exit)"
echo ""

exec "$APP_PATH"