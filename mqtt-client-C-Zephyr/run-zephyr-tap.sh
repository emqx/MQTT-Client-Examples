#!/bin/bash
# 不使用 set -e，以支持交互式 fg 切换

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
BUILD_DIR=${BUILD_DIR:-"$SCRIPT_DIR/build-tap-tls"}
APP_PATH=${APP_PATH:-"$BUILD_DIR/zephyr/zephyr.exe"}

if [[ ! -x "$APP_PATH" ]]; then
    echo "❌ 未找到 TAP/TLS 可执行文件: $APP_PATH"
    echo "   请先执行:"
    echo "   west build -d build-tap-tls -p always -b native_sim/native/64 . -- -DOVERLAY_CONFIG=prj-tap-tls.conf"
    exit 1
fi

cleanup() {
    echo ""
    echo "🛑 关闭 Zephyr..."
    kill $ZEPHYR_PID 2>/dev/null
    wait $ZEPHYR_PID 2>/dev/null
    echo "✅ Zephyr 已关闭"
    exit 0
}
trap cleanup INT TERM

# ─── 清理上次运行的残留 ───
echo "🧹 清理旧进程和接口..."
pkill -9 -f zephyr.exe 2>/dev/null || true
sleep 1
ip link del zeth 2>/dev/null || true

# ─── 启动 Zephyr ───
echo "🟢 启动 Zephyr..."
set -m  # 启用作业控制（fg 必需）
"$APP_PATH" &
ZEPHYR_PID=$!

# 确认进程存活
sleep 1
if ! kill -0 $ZEPHYR_PID 2>/dev/null; then
    echo "❌ Zephyr 启动失败（进程已退出）"
    exit 1
fi

# ─── 等待 zeth 接口 ───
echo "⚙️  等待 zeth 接口就绪..."
for i in $(seq 1 10); do
    ip link show zeth 2>/dev/null && break
    echo "   等待 zeth ($i/10)..."
    sleep 1
done

if ! ip link show zeth > /dev/null 2>&1; then
    echo "❌ zeth 接口超时未出现"
    kill $ZEPHYR_PID 2>/dev/null
    exit 1
fi

# ─── 配置网络 ───
echo "⚙️  配置 zeth 接口..."
ip addr add 192.0.2.2/24 dev zeth 2>/dev/null || true
ip link set zeth up 2>/dev/null || true
sysctl -w net.ipv4.ip_forward=1 2>/dev/null || \
    echo "   ⚠️  sysctl 失败（容器内可能只读），依赖 docker run --sysctl 或宿主机已开启"

iptables -t nat -C POSTROUTING -s 192.0.2.0/24 -j MASQUERADE 2>/dev/null || \
    iptables -t nat -A POSTROUTING -s 192.0.2.0/24 -j MASQUERADE
iptables -C FORWARD -i zeth -o eth0 -j ACCEPT 2>/dev/null || \
    iptables -A FORWARD -i zeth -o eth0 -j ACCEPT
iptables -C FORWARD -i eth0 -o zeth -j ACCEPT 2>/dev/null || \
    iptables -A FORWARD -i eth0 -o zeth -j ACCEPT

echo "✅ 网络就绪 | Zephyr: 192.0.2.1 | Host: 192.0.2.2"
echo "   (输入命令到 uart:~$，Ctrl+C 退出)"
echo ""

# ─── 切换到前台交互 ───
fg %1