# ESP32 温度图表 Web 应用程序

这个基于 Flask 的 Web 应用程序用于显示通过 MQTT 从 ESP32 设备发送的温度数据。它包括一个用于可视化温度图表的 Web 界面和一个获取温度数据的 API。

### 系统要求

- Python 3.8 或更高版本
- 虚拟环境工具（如 `virtualenv` 或 `conda`）
- MQTT 代理凭证

### 本地设置

1. **克隆仓库：**

   ```
   git clone [仓库 URL]
   cd [仓库名称]
   ```

2. **创建虚拟环境：**

   ```
   python -m venv venv
   source venv/bin/activate  # Windows 上: venv\Scripts\activate
   ```

3. **安装依赖：**

   ```
   pip install -r requirements.txt
   ```

4. **在 `app.py` 中配置 MQTT 设置：**
   修改 `app.py` 中的 MQTT 配置设置：

    - `MQTT_BROKER_URL`
    - `MQTT_BROKER_PORT`
    - `MQTT_USERNAME`
    - `MQTT_PASSWORD`
    - `MQTT_CLIENT_ID`

5. **运行应用程序：**

   ```
   python app.py
   ```

   在 `http://localhost:8080` 访问 Web 界面。

### 部署到 Fly.io

1. **安装 Fly.io CLI：**
   按照 [Fly.io 文档](https://fly.io/docs/getting-started/installing-flyctl/) 中的说明进行操作。

2. **登录 Fly.io：**

   ```
   flyctl auth login
   ```

3. **初始化 Fly.io 应用：**

   ```
   flyctl launch
   ```

4. **部署应用程序：**

   ```
   flyctl deploy
   ```

5. **验证部署：**

   ```
   flyctl status
   ```

   访问 Fly.io 提供的 URL 以访问您的应用程序。
