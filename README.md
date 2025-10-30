# Project Shield - ThingsBoard OTA Firmware Update

![Version](https://img.shields.io/badge/version-1.0.1-blue.svg)
![Platform](https://img.shields.io/badge/platform-ESP32-green.svg)
![ThingsBoard](https://img.shields.io/badge/ThingsBoard-Cloud-orange.svg)

A robust ESP32 firmware update system using ThingsBoard IoT platform's Over-The-Air (OTA) update capabilities. This project enables remote firmware management and updates for ESP32 devices through the ThingsBoard device profile.

## 🌟 Features

- **OTA Firmware Updates**: Remotely update ESP32 firmware through ThingsBoard
- **Device Profile Integration**: Fetches firmware updates from ThingsBoard device profile
- **Encrypted/Unencrypted Connections**: Supports both MQTT (1883) and MQTTS (8883)
- **Update Progress Tracking**: Real-time progress monitoring during firmware updates
- **Automatic Retry Mechanism**: Configurable retry attempts for failed updates
- **WiFi Auto-Reconnect**: Maintains stable connection to ThingsBoard server

## 📋 Prerequisites

### Hardware
- ESP32 development board
- USB cable for initial programming
- Stable WiFi network

### Software
- Arduino IDE (1.8.x or later) or PlatformIO
- ESP32 board support package
- Required libraries (see Installation section)

### ThingsBoard Account
- ThingsBoard Cloud account or self-hosted instance
- Device created in ThingsBoard
- Device access token

## 🔧 Installation

### 1. Install Required Libraries

Install the following libraries through Arduino IDE Library Manager or PlatformIO:

```
- WiFi (built-in with ESP32)
- WiFiClientSecure (built-in with ESP32)
- ThingsBoard (by ThingsBoard Team)
- ArduinoHttpClient
- Arduino_MQTT_Client
```

### 2. Clone Repository

```bash
git clone https://github.com/yourusername/project-shield.git
cd project-shield
```

### 3. Configure Project

Edit the following constants in the main `.cpp` file:

```cpp
// WiFi Credentials
constexpr char WIFI_SSID[]       = "Your_WiFi_SSID";
constexpr char WIFI_PASSWORD[]   = "Your_WiFi_Password";

// ThingsBoard Configuration
constexpr char TOKEN[]           = "YOUR_DEVICE_TOKEN";
constexpr char THINGSBOARD_SERVER[] = "eu.thingsboard.cloud"; // or your server

// Firmware Information
constexpr char CURRENT_FIRMWARE_TITLE[] = "Project Shield";
constexpr char CURRENT_FIRMWARE_VERSION[] = "1.0.1";
```

### 4. Configure Encryption (Optional)

For secure MQTT connection using port 8883:

```cpp
#define ENCRYPTED true  // Enable encryption

// Add ThingsBoard Root CA Certificate
constexpr char ROOT_CERT[] = R"(-----BEGIN CERTIFICATE-----
YOUR_ROOT_CA_CERTIFICATE_HERE
-----END CERTIFICATE-----)";
```

To get the Root CA certificate for ThingsBoard Cloud:
```bash
openssl s_client -showcerts -connect eu.thingsboard.cloud:8883 </dev/null 2>/dev/null | openssl x509 -outform PEM
```

## 🚀 Usage

### Initial Setup

1. **Upload Initial Firmware**
   - Connect ESP32 via USB
   - Compile and upload the code
   - Open Serial Monitor (115200 baud) to verify connection

2. **ThingsBoard Device Configuration**
   - Log in to ThingsBoard
   - Navigate to your device
   - Go to Device Profile → Firmware
   - Upload new firmware binary (.bin file)

### Performing OTA Update

1. **Upload New Firmware to ThingsBoard**
   - Compile your updated firmware
   - Export compiled binary (`.bin` file)
   - Upload to ThingsBoard device profile
   - Increment version number (e.g., 1.0.2)

2. **Trigger Update**
   - The device automatically checks for updates on connection
   - Device compares its version with ThingsBoard version
   - If newer version available, download begins automatically
   - Monitor progress via Serial Monitor

3. **Update Process**
   ```
   Progress 0.00%
   Progress 25.50%
   Progress 51.00%
   Progress 76.50%
   Progress 100.00%
   Done, Reboot now
   ```

## 📡 Configuration Parameters

| Parameter | Default | Description |
|-----------|---------|-------------|
| `ENCRYPTED` | false | Enable/disable MQTT encryption |
| `THINGSBOARD_PORT` | 1883 / 8883 | MQTT port (unencrypted/encrypted) |
| `MAX_MESSAGE_SIZE` | 512 | MQTT message buffer size |
| `FIRMWARE_FAILURE_RETRIES` | 12 | Update retry attempts |
| `FIRMWARE_PACKET_SIZE` | 4096 | Firmware download packet size |

## 🔍 Troubleshooting

### Connection Issues

**Problem**: Cannot connect to ThingsBoard
```
Failed to connect
```

**Solutions**:
- Verify device token is correct
- Check WiFi credentials
- Ensure ThingsBoard server URL is correct
- For encrypted connection, verify Root CA certificate

### OTA Update Failures

**Problem**: Firmware download fails
```
Downloading firmware failed
```

**Solutions**:
- Check available flash memory (ESP32 requires partition for OTA)
- Verify firmware binary is compatible with ESP32
- Increase `FIRMWARE_FAILURE_RETRIES`
- Check network stability
- Reduce `FIRMWARE_PACKET_SIZE` if network is unstable

### Memory Issues

**Problem**: Device crashes during update

**Solutions**:
- Reduce `MAX_MESSAGE_SIZE`
- Reduce `FIRMWARE_PACKET_SIZE`
- Ensure ESP32 has proper OTA partition scheme

## 📊 Serial Monitor Output

Normal operation produces output like:
```
Connecting to AP ...
........
Connected to AP
Connecting to: (eu.thingsboard.cloud) with token (joah0XYBuxjQBk9JablT)
Firmware Update...
Progress 0.00%
Progress 12.50%
...
Progress 100.00%
Done, Reboot now
```

## 🛠️ Development

### Building New Firmware

1. Make code changes
2. Update version in code:
   ```cpp
   constexpr char CURRENT_FIRMWARE_VERSION[] = "1.0.2";
   ```
3. Compile and export binary
4. Upload to ThingsBoard

### Custom Callbacks

The code includes three callbacks for update lifecycle:

```cpp
void update_starting_callback() {
    // Called when update begins
}

void progress_callback(const size_t & current, const size_t & total) {
    // Called during download
}

void finished_callback(const bool & success) {
    // Called when update completes
}
```

## 📝 License

This project is licensed under the MIT License - see the LICENSE file for details.

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 🙏 Acknowledgments

- [ThingsBoard](https://thingsboard.io/) - IoT Platform
- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32)
- ThingsBoard Arduino SDK

---

**Note**: Always test OTA updates in a development environment before deploying to production devices. Failed OTA updates can brick devices if not handled properly.