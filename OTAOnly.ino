#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <array>                     // <-- لاستخدام std::array
#define THINGSBOARD_ENABLE_DEBUG 1   // اختياري لوج مفيد
#include <Arduino_MQTT_Client.h>
#include <OTA_Firmware_Update.h>
#include <ThingsBoard.h>
#include <Espressif_Updater.h>

// Whether the given script is using encryption or not.
#define ENCRYPTED false  // لو فشل 1883، خلّيها true واستعمل 8883 + Root CA

constexpr char CURRENT_FIRMWARE_TITLE[] = "Project Shield";
constexpr char CURRENT_FIRMWARE_VERSION[] = "1.0.1";
constexpr uint8_t  FIRMWARE_FAILURE_RETRIES = 12U;
constexpr uint16_t FIRMWARE_PACKET_SIZE    = 4096U;

constexpr char WIFI_SSID[]       = "Redmi 9T";
constexpr char WIFI_PASSWORD[]   = "00000000";
constexpr char TOKEN[]           = "joah0XYBuxjQBk9JablT";
constexpr char THINGSBOARD_SERVER[] = "eu.thingsboard.cloud";

#if ENCRYPTED
constexpr uint16_t THINGSBOARD_PORT = 8883U;
#else
constexpr uint16_t THINGSBOARD_PORT = 1883U;
#endif

constexpr uint16_t MAX_MESSAGE_SIZE  = 512U;
constexpr uint32_t SERIAL_DEBUG_BAUD = 115200U;

#if ENCRYPTED
WiFiClientSecure espClient;
constexpr char ROOT_CERT[] = R"(-----BEGIN CERTIFICATE-----
[PUT THINGSBOARD.CLOUD ROOT CA HERE]
-----END CERTIFICATE-----)";
#else
WiFiClient espClient;
#endif

Arduino_MQTT_Client mqttClient(espClient);
OTA_Firmware_Update<> ota;

// ⬇️ مرّر الـ APIs كـ std::array (Container) مش كمؤشر
const std::array<IAPI_Implementation*, 1U> apis = { &ota };

// إنشاء الـ ThingsBoard: (recv, send, max_stack, apis-container)
ThingsBoard tb(
  mqttClient,
  MAX_MESSAGE_SIZE,              // receive buffer
  MAX_MESSAGE_SIZE,              // send buffer
  Default_Max_Stack_Size,        // max stack size required by SDK
  apis                           // <-- Container فيه الـ APIs
);

Espressif_Updater<> updater;

bool currentFWSent = false;
bool updateRequestSent = false;

void InitWiFi() {
  Serial.println("Connecting to AP ...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
#if ENCRYPTED
  espClient.setCACert(ROOT_CERT);
#endif
}

bool reconnect() {
  if (WiFi.status() == WL_CONNECTED) return true;
  InitWiFi();
  return true;
}

void update_starting_callback() {}

void finished_callback(const bool & success) {
  if (success) {
    Serial.println("Done, Reboot now");
    ESP.restart();  // أنسب مع Arduino
  } else {
    Serial.println("Downloading firmware failed");
  }
}

void progress_callback(const size_t & current, const size_t & total) {
  Serial.printf("Progress %.2f%%\n", static_cast<float>(current * 100U) / total);
}

void setup() {
  Serial.begin(SERIAL_DEBUG_BAUD);
  delay(1000);
  InitWiFi();
}

void loop() {
  delay(1000);

  if (!reconnect()) return;

  if (!tb.connected()) {
    Serial.printf("Connecting to: (%s) with token (%s)\n", THINGSBOARD_SERVER, TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN, THINGSBOARD_PORT)) {
      Serial.println("Failed to connect");
      return;
    }
  }

  if (!currentFWSent) {
    currentFWSent = ota.Firmware_Send_Info(CURRENT_FIRMWARE_TITLE, CURRENT_FIRMWARE_VERSION);
  }

  if (!updateRequestSent) {
    Serial.println("Firmware Update...");
    const OTA_Update_Callback callback(
      CURRENT_FIRMWARE_TITLE, CURRENT_FIRMWARE_VERSION, &updater,
      &finished_callback, &progress_callback, &update_starting_callback,
      FIRMWARE_FAILURE_RETRIES, FIRMWARE_PACKET_SIZE
    );
    updateRequestSent = ota.Start_Firmware_Update(callback);
  }

  tb.loop();
}
