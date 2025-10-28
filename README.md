# ESP32-C6 ThingsBoard Cloud OTA Firmware Updater

A robust Over-The-Air (OTA) firmware update system for ESP32-C6 microcontrollers using ThingsBoard Cloud IoT platform.

## 🚀 Features

- **Automatic OTA Updates**: Checks for firmware updates every 60 seconds
- **ThingsBoard Cloud Integration**: Connects to ThingsBoard EU Cloud endpoint
- **Firmware Version Management**: Reports current firmware version and state
- **Secure HTTPS Communication**: Uses WiFiClientSecure for encrypted data transfer
- **Device Token Validation**: Built-in token testing functionality
- **Status Reporting**: Reports OTA success/failure status back to ThingsBoard
- **WiFi Auto-Reconnect**: Handles WiFi disconnections automatically

## 📋 Prerequisites

### Hardware
- ESP32-C6 development board
- USB cable for programming
- WiFi network access

### Software
- Arduino IDE (1.8.x or 2.x)
- ESP32 Board Support Package
- Required Libraries:
  - `WiFi.h`
  - `WiFiClientSecure.h`
  - `HTTPClient.h`
  - `ArduinoJson.h` (v6.x)
  - `Update.h`

### ThingsBoard Account
- Active account on [ThingsBoard EU Cloud](https://eu.thingsboard.cloud)
- Device provisioned with access token

## 🔧 Installation

### 1. Install Arduino Libraries

Open Arduino IDE and install the following libraries via Library Manager:

```
Tools > Manage Libraries...
```

Search and install:
- **ArduinoJson** by Benoit Blanchon (version 6.x)

### 2. Install ESP32 Board Support

Add ESP32 board support to Arduino IDE:

1. Go to `File > Preferences`
2. Add to "Additional Board Manager URLs":
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Go to `Tools > Board > Board Manager`
4. Search for "ESP32" and install

### 3. Clone This Repository

```bash
git clone https://github.com/yourusername/esp32-thingsboard-ota.git
cd esp32-thingsboard-ota
```

## ⚙️ Configuration

### 1. WiFi Settings

Edit the following lines in the code:

```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

### 2. ThingsBoard Device Token

1. Log in to [ThingsBoard EU Cloud](https://eu.thingsboard.cloud)
2. Navigate to **Devices**
3. Create a new device or select existing device
4. Copy the **Access Token**
5. Update the code:

```cpp
const char* DEVICE_TOKEN = "YOUR_DEVICE_TOKEN";
```

### 3. Firmware Version

Update your firmware metadata:

```cpp
const char* CURRENT_FW_TITLE = "YOUR_FIRMWARE_NAME";
const char* CURRENT_FW_VERSION = "1.0.0";  // Update this with each release
```

## 📤 Setting Up OTA Updates in ThingsBoard

### 1. Prepare New Firmware

1. Update the `CURRENT_FW_VERSION` in your code (e.g., "1.0.1")
2. Compile and export binary:
   - `Sketch > Export Compiled Binary`
3. Upload the `.bin` file to a publicly accessible URL (GitHub Releases, cloud storage, etc.)

### 2. Configure Shared Attributes

In ThingsBoard dashboard:

1. Go to your device
2. Navigate to **Attributes** tab
3. Select **Shared attributes**
4. Add the following attributes:

| Attribute Key | Attribute Value |
|--------------|-----------------|
| `fw_title` | Your firmware name |
| `fw_version` | New version (e.g., "1.0.1") |
| `fw_url` | Direct URL to .bin file |

**Example:**
```json
{
  "fw_title": "TEST",
  "fw_version": "1.0.1",
  "fw_url": "https://example.com/firmware/v1.0.1.bin"
}
```

### 3. Monitor Update Process

Watch the Serial Monitor (115200 baud) to see:
- Update detection
- Download progress
- Flash status
- Device restart

## 📊 Telemetry Data

The device reports the following telemetry to ThingsBoard:

| Key | Description |
|-----|-------------|
| `current_fw_title` | Current firmware name |
| `current_fw_version` | Current firmware version |
| `fw_state` | Firmware state (UPDATED, UPDATING, FAILED) |
| `device_type` | Device model (ESP32-C6) |
| `ota_status` | OTA update result (SUCCESS/FAILED) |
| `timestamp` | Milliseconds since boot |

## 🐛 Troubleshooting

### Device Token Validation Fails (401 Error)

**Solution:**
1. Verify you're using EU Cloud endpoint: `eu.thingsboard.cloud`
2. Confirm device exists in ThingsBoard
3. Copy the correct access token from device credentials
4. Check for extra spaces in the token string

### WiFi Connection Issues

**Solution:**
- Verify SSID and password
- Check WiFi signal strength
- Ensure 2.4GHz network (ESP32 doesn't support 5GHz)

### OTA Update Fails

**Solution:**
- Ensure firmware URL is publicly accessible
- Verify `.bin` file is not corrupted
- Check available flash space
- Confirm firmware is compiled for ESP32-C6

### Certificate Verification Errors

The code uses `client.setInsecure()` to skip certificate verification. For production:

```cpp
// Use root certificate instead
const char* root_ca = "-----BEGIN CERTIFICATE-----\n"
                      "...\n"
                      "-----END CERTIFICATE-----\n";
client.setCACert(root_ca);
```

## 📝 Serial Monitor Output

Expected output at 115200 baud:

```
=== ESP32-C6 ThingsBoard OTA Demo ===
Firmware Version: 1.0.0
Target: EU ThingsBoard Cloud
Connecting to WiFi...
WiFi connected!

=== Testing Device Token on EU Cloud ===
✅ SUCCESS: Device token is VALID!

Reporting firmware info to EU Cloud...
✅ Firmware info reported successfully!

Checking for OTA updates on EU Cloud...
New firmware detected: TEST (1.0.1)
Starting OTA download...
✅ OTA update completed successfully!
Restarting in 3 seconds...
```

## 🔒 Security Considerations

⚠️ **Important Security Notes:**

1. **Device Token**: Keep your device token secret. Don't commit it to public repositories
2. **HTTPS**: This code uses `setInsecure()` for testing. For production, implement proper certificate validation
3. **Firmware URLs**: Ensure firmware binaries are hosted securely with access controls
4. **WiFi Credentials**: Store credentials securely, consider using ESP32 NVS or secure elements

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📧 Support

If you encounter any issues or have questions:

- Open an issue on GitHub
- Check ThingsBoard [documentation](https://thingsboard.io/docs/)
- Visit ESP32 [community forums](https://esp32.com/)

## 🙏 Acknowledgments

- [ThingsBoard](https://thingsboard.io/) - IoT Platform
- [Espressif](https://www.espressif.com/) - ESP32 microcontrollers
- [ArduinoJson](https://arduinojson.org/) - JSON library

---

**Made with ❤️ for IoT enthusiasts**
