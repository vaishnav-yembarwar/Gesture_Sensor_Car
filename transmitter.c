#include <Wire.h>
#include <esp_now.h>
#include <WiFi.h>
#include <MPU6050.h>

// Create MPU6050 object
MPU6050 mpu;

// Structure to send gesture data
typedef struct struct_message {
  int xAxis;
  int yAxis;
} struct_message;

struct_message gestureData;

// 🔁 Replace this with your RECEIVER ESP32's MAC address
uint8_t receiverMac[] = {0x24, 0x6F, 0x28, 0xAB, 0xCD, 0xEF};

// Callback after data is sent
void onSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);

  // Start I2C for MPU6050 (SDA = 21, SCL = 22)
  Wire.begin(21, 22);
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed!");
    while (1);
  } else {
    Serial.println("MPU6050 connected.");
  }

  // Initialize WiFi in station mode
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed!");
    return;
  }

  // Register the send callback
  esp_now_register_send_cb(onSent);

  // Register peer
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  gestureData.xAxis = ax;
  gestureData.yAxis = ay;

  // Send data to receiver
  esp_now_send(receiverMac, (uint8_t *)&gestureData, sizeof(gestureData));

  // Print for debug
  Serial.print("Sent - X: ");
  Serial.print(ax);
  Serial.print(" | Y: ");
  Serial.print(ay);

  // Optional: gesture direction detection
  if (ay > 10000) {
    Serial.println(" -> Forward");
  } else if (ay < -10000) {
    Serial.println(" -> Backward");
  } else if (ax > 10000) {
    Serial.println(" -> Right");
  } else if (ax < -10000) {
    Serial.println(" -> Left");
  } else {
    Serial.println(" -> Stop");
  }

  delay(200); // Adjust for responsiveness
}
