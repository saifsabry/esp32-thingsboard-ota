#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Update.h>

// WiFi credentials
const char* ssid = "Redmi 9T";
const char* password = "00000000";

// ThingsBoard EU Cloud config
const char* THINGSBOARD_HOST = "eu.thingsboard.cloud";  // CHANGED TO EU ENDPOINT
const int THINGSBOARD_PORT = 443;
const char* DEVICE_TOKEN = "joah0XYBuxjQBk9JablT";

// Current firmware info
const char* CURRENT_FW_TITLE = "TEST";
const char* CURRENT_FW_VERSION = "1.0.0";

// Update intervals
const unsigned long CHECK_INTERVAL = 60000;
unsigned long lastCheck = 0;

WiFiClientSecure client;
HTTPClient http;

void connectWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 20) {
    delay(500);
    Serial.print(".");
    tries++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi failed.");
  }
}

void testDeviceToken() {
  Serial.println("\n=== Testing Device Token on EU Cloud ===");
  Serial.print("Using Host: ");
  Serial.println(THINGSBOARD_HOST);
  Serial.print("Token: ");
  Serial.println(DEVICE_TOKEN);
  
  String testUrl = "https://" + String(THINGSBOARD_HOST) + "/api/v1/" + String(DEVICE_TOKEN) + "/attributes";
  Serial.println("Testing URL: " + testUrl);
  
  http.begin(client, testUrl);
  int code = http.GET();
  String response = http.getString();
  
  Serial.println("HTTP Response Code: " + String(code));
  Serial.println("Response: " + response);
  
  if (code == 200) {
    Serial.println("✅ SUCCESS: Device token is VALID!");
  } else if (code == 401) {
    Serial.println("❌ FAIL: Device token is INVALID!");
    Serial.println("Please verify:");
    Serial.println("1. Go to: https://eu.thingsboard.cloud");
    Serial.println("2. Check if device exists in Devices list");
    Serial.println("3. Copy the correct device token");
  } else {
    Serial.println("⚠️  Other error: " + String(code));
    Serial.println("Error details: " + http.errorToString(code));
  }
  
  http.end();
  Serial.println("=== End Token Test ===\n");
}

void sendFirmwareInfo() {
  Serial.println("Reporting firmware info to EU Cloud...");
  
  DynamicJsonDocument doc(256);
  doc["current_fw_title"] = CURRENT_FW_TITLE;
  doc["current_fw_version"] = CURRENT_FW_VERSION;
  doc["fw_state"] = "UPDATED";
  doc["device_type"] = "ESP32-C6";
  doc["timestamp"] = millis();

  String payload;
  serializeJson(doc, payload);

  String url = "https://" + String(THINGSBOARD_HOST) + "/api/v1/" + String(DEVICE_TOKEN) + "/telemetry";
  
  Serial.println("Sending to: " + url);
  Serial.println("Payload: " + payload);
  
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "application/json");
  
  int code = http.POST(payload);
  String response = http.getString();
  
  Serial.println("HTTP Response Code: " + String(code));
  Serial.println("Server Response: " + response);
  
  http.end();

  if (code == 200) {
    Serial.println("✅ Firmware info reported successfully to EU Cloud!");
  } else {
    Serial.println("❌ Failed to report firmware info.");
    Serial.println("Error: " + http.errorToString(code));
  }
}

void checkForUpdate() {
  Serial.println("Checking for OTA updates on EU Cloud...");
  String url = "https://" + String(THINGSBOARD_HOST) + "/api/v1/" + String(DEVICE_TOKEN) + "/attributes";

  Serial.println("Request URL: " + url);
  
  http.begin(client, url);
  int code = http.GET();
  String response = http.getString();

  Serial.println("OTA Check - HTTP Code: " + String(code));
  Serial.println("Response: " + response);

  if (code == 200) {
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, response);
    
    if (!error) {
      // Check for firmware attributes in shared section
      if (doc.containsKey("shared")) {
        JsonObject shared = doc["shared"];
        const char* fw_title = shared["fw_title"];
        const char* fw_version = shared["fw_version"];
        const char* fw_url = shared["fw_url"];

        if (fw_title && fw_version && fw_url) {
          Serial.println("Found firmware attributes:");
          Serial.println("  Title: " + String(fw_title));
          Serial.println("  Version: " + String(fw_version));
          Serial.println("  URL: " + String(fw_url));
          
          if (strcmp(fw_version, CURRENT_FW_VERSION) != 0) {
            Serial.printf("New firmware detected: %s (%s)\n", fw_title, fw_version);
            performUpdate(fw_url, fw_version);
          } else {
            Serial.println("Already running latest firmware.");
          }
        } else {
          Serial.println("No firmware attributes found in shared attributes.");
          Serial.println("Available shared attributes:");
          for (JsonPair kv : shared) {
            Serial.println("  " + String(kv.key().c_str()) + ": " + String(kv.value().as<String>()));
          }
        }
      } else {
        Serial.println("No shared attributes found.");
      }
    } else {
      Serial.println("JSON parsing error: " + String(error.c_str()));
    }
  } else {
    Serial.println("OTA check failed: " + http.errorToString(code));
  }
  http.end();
}

void performUpdate(const char* fw_url, const char* newVersion) {
  Serial.println("Starting OTA download from: " + String(fw_url));

  http.begin(client, fw_url);
  int code = http.GET();

  if (code == HTTP_CODE_OK) {
    int contentLength = http.getSize();
    Serial.println("Firmware size: " + String(contentLength) + " bytes");
    
    WiFiClient* stream = http.getStreamPtr();

    if (Update.begin(contentLength)) {
      Serial.println("Starting firmware flash...");
      
      size_t written = Update.writeStream(*stream);
      Serial.println("Written: " + String(written) + " bytes");
      
      if (written == contentLength && Update.end()) {
        Serial.println("✅ OTA update completed successfully!");
        reportOtaStatus("SUCCESS", newVersion);
        Serial.println("Restarting in 3 seconds...");
        delay(3000);
        ESP.restart();
      } else {
        Serial.println("❌ OTA update failed during flash!");
        reportOtaStatus("FAILED", newVersion);
      }
    } else {
      Serial.println("❌ Not enough space for OTA update");
      reportOtaStatus("FAILED", newVersion);
    }
  } else {
    Serial.println("❌ Failed to download firmware: " + String(code));
    reportOtaStatus("FAILED", newVersion);
  }
  http.end();
}

void reportOtaStatus(const char* status, const char* version) {
  Serial.println("Reporting OTA status: " + String(status));
  
  DynamicJsonDocument doc(256);
  doc["ota_status"] = status;
  doc["fw_version"] = version;
  doc["timestamp"] = millis();

  String payload;
  serializeJson(doc, payload);

  String url = "https://" + String(THINGSBOARD_HOST) + "/api/v1/" + String(DEVICE_TOKEN) + "/telemetry";
  
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  int code = http.POST(payload);
  
  if (code == 200) {
    Serial.println("✅ OTA status reported successfully");
  } else {
    Serial.println("❌ Failed to report OTA status: " + String(code));
  }
  http.end();
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("=== ESP32-C6 ThingsBoard OTA Demo ===");
  Serial.println("Firmware Version: " + String(CURRENT_FW_VERSION));
  Serial.println("Target: EU ThingsBoard Cloud");
  
  // Configure secure client
  client.setInsecure(); // Skip certificate verification for testing
  client.setTimeout(15000);
  
  connectWiFi();
  
  // Test connection first
  testDeviceToken();
  
  // Report firmware info
  sendFirmwareInfo();
  
  Serial.println("Setup completed. Waiting for OTA checks...");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");
    connectWiFi();
  }

  if (millis() - lastCheck > CHECK_INTERVAL) {
    checkForUpdate();
    lastCheck = millis();
  }
  
  delay(1000);
}