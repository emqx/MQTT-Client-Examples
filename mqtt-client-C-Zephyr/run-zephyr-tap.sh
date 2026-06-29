#!/bin/bash
# Do NOT use set -e; interactive fg switching must not abort the script

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
BUILD_DIR=${BUILD_DIR:-"$SCRIPT_DIR/build-tap-tls"}
APP_PATH=${APP_PATH:-"$BUILD_DIR/zephyr/zephyr.exe"}

if [[ ! -x "$APP_PATH" ]]; then
    echo "❌ TAP/TLS executable not found: $APP_PATH"
    echo "   Run the build first:"
    echo "   west build -d build-tap-tls -p always -b native_sim/native/64 . -- -DOVERLAY_CONFIG=prj-tap-tls.conf"
    exit 1
fi

cleanup() {
    echo ""
    echo "🛑 Stopping Zephyr..."
    kill $ZEPHYR_PID 2>/dev/null
    wait $ZEPHYR_PID 2>/dev/null
    echo "✅ Zephyr stopped"
    exit 0
}
trap cleanup INT TERM

# ─── Clean up leftover processes and interfaces ───
echo "🧹 Cleaning up old processes and interfaces..."
pkill -9 -f zephyr.exe 2>/dev/null || true
sleep 1
ip link del zeth 2>/dev/null || true

# ─── Start Zephyr ───
echo "🟢 Starting Zephyr..."
set -m  # enable job control (required for fg)
"$APP_PATH" &
ZEPHYR_PID=$!

# Verify the process is alive
sleep 1
if ! kill -0 $ZEPHYR_PID 2>/dev/null; then
    echo "❌ Zephyr failed to start (process already exited)"
    exit 1
fi

# ─── Wait for the zeth interface ───
echo "⚙️  Waiting for zeth interface..."
for i in $(seq 1 10); do
    ip link show zeth 2>/dev/null && break
    echo "   Waiting for zeth ($i/10)..."
    sleep 1
done

if ! ip link show zeth > /dev/null 2>&1; then
    echo "❌ zeth interface did not appear within timeout"
    kill $ZEPHYR_PID 2>/dev/null
    exit 1
fi

# ─── Configure networking ───
echo "⚙️  Configuring zeth interface..."
ip addr add 192.0.2.2/24 dev zeth 2>/dev/null || true
ip link set zeth up 2>/dev/null || true
sysctl -w net.ipv4.ip_forward=1 2>/dev/null || \
    echo "   ⚠️  sysctl failed (container may be read-only); ensure forwarding is enabled on the host"

iptables -t nat -C POSTROUTING -s 192.0.2.0/24 -j MASQUERADE 2>/dev/null || \
    iptables -t nat -A POSTROUTING -s 192.0.2.0/24 -j MASQUERADE
iptables -C FORWARD -i zeth -o eth0 -j ACCEPT 2>/dev/null || \
    iptables -A FORWARD -i zeth -o eth0 -j ACCEPT
iptables -C FORWARD -i eth0 -o zeth -j ACCEPT 2>/dev/null || \
    iptables -A FORWARD -i eth0 -o zeth -j ACCEPT

echo "✅ Network ready | Zephyr: 192.0.2.1 | Host: 192.0.2.2"
echo "   (Type commands at uart:~$, Ctrl+C to exit)"
echo ""

# ─── Bring Zephyr to foreground for interactive use ───
fg %1