# ESP32 Temperature Chart Web Application

This Flask-based web application displays temperature data sent from an ESP32 device using MQTT. It includes a web interface for visualizing the temperature chart and an API to fetch temperature data.

### Requirements

- Python 3.8 or later
- Virtual environment tool (like `virtualenv` or `conda`)
- MQTT Broker credentials

### Local Setup

1. **Clone the Repository:**
   ```
   git clone [Repository URL]
   cd [Repository Name]
   ```

2. **Create a Virtual Environment:**
   ```
   python -m venv venv
   source venv/bin/activate  # On Windows: venv\Scripts\activate
   ```

3. **Install Dependencies:**
   ```
   pip install -r requirements.txt
   ```

4. **Configure MQTT Settings in `app.py`:**
   Modify the MQTT configuration settings in `app.py`:
   - `MQTT_BROKER_URL`
   - `MQTT_BROKER_PORT`
   - `MQTT_USERNAME`
   - `MQTT_PASSWORD`
   - `MQTT_CLIENT_ID`

5. **Run the Application:**
   ```
   python app.py
   ```
   Access the web interface at `http://localhost:8080`.

### Deploying to Fly.io

1. **Install Fly.io CLI:**
   Follow the instructions on [Fly.io documentation](https://fly.io/docs/getting-started/installing-flyctl/).

2. **Login to Fly.io:**
   ```
   flyctl auth login
   ```

3. **Initialize Fly.io App:**
   ```
   flyctl launch
   ```

4. **Deploy the App:**
   ```
   flyctl deploy
   ```

5. **Verify Deployment:**
   ```
   flyctl status
   ```
   Visit the URL provided by Fly.io to access your application.
