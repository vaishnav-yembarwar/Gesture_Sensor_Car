// Data structure to receive gesture commands
typedef struct struct_message {
  int xAxis;
  int yAxis;
} struct_message;

struct_message incomingData;

void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingDataRaw, int len) {
  memcpy(&incomingData, incomingDataRaw, sizeof(incomingData));

  int x = incomingData.xAxis;
  int y = incomingData.yAxis;

  Serial.print("X: "); Serial.print(x);
  Serial.print(" | Y: "); Serial.println(y);

  if (y > 10000) {
    moveForward();
    Serial.println("Forward");
  } else if (y < -10000) {
    moveBackward();
    Serial.println("Backward");
  } else if (x > 10000) {
    turnRight();
    Serial.println("Right");
  } else if (x < -10000) {
    turnLeft();
    Serial.println("Left");
  } else {
    stopMotors();
    Serial.println("Stop");
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);

  // Set motor pins as OUTPUT
  int pins[] = {
    enA_F, in1_F, in2_F, in3_F, in4_F, enB_F,
    enA_B, in1_B, in2_B, in3_B, in4_B, enB_B
  };

  for (int i = 0; i < sizeof(pins) / sizeof(pins[0]); i++) {
    pinMode(pins[i], OUTPUT);
  }

  stopMotors();
}

void loop() {
  // Nothing here - actions are triggered by gesture data
}

// -------- Motor Control Functions --------

void moveForward() {
  digitalWrite(in1_F, HIGH); digitalWrite(in2_F, LOW);
  digitalWrite(in3_F, HIGH); digitalWrite(in4_F, LOW);
  analogWrite(enA_F, 200); analogWrite(enB_F, 200);

  digitalWrite(in1_B, HIGH); digitalWrite(in2_B, LOW);
  digitalWrite(in3_B, HIGH); digitalWrite(in4_B, LOW);
  analogWrite(enA_B, 200); analogWrite(enB_B, 200);
}

void moveBackward() {
  digitalWrite(in1_F, LOW); digitalWrite(in2_F, HIGH);
  digitalWrite(in3_F, LOW); digitalWrite(in4_F, HIGH);
  analogWrite(enA_F, 200); analogWrite(enB_F, 200);

  digitalWrite(in1_B, LOW); digitalWrite(in2_B, HIGH);
  digitalWrite(in3_B, LOW); digitalWrite(in4_B, HIGH);
  analogWrite(enA_B, 200); analogWrite(enB_B, 200);
}

void turnLeft() {
  digitalWrite(in1_F, LOW); digitalWrite(in2_F, HIGH);
  digitalWrite(in3_F, HIGH); digitalWrite(in4_F, LOW);
  analogWrite(enA_F, 180); analogWrite(enB_F, 180);

  digitalWrite(in1_B, LOW); digitalWrite(in2_B, HIGH);
  digitalWrite(in3_B, HIGH); digitalWrite(in4_B, LOW);
  analogWrite(enA_B, 180); analogWrite(enB_B, 180);
}

void turnRight() {
  digitalWrite(in1_F, HIGH); digitalWrite(in2_F, LOW);
  digitalWrite(in3_F, LOW); digitalWrite(in4_F, HIGH);
  analogWrite(enA_F, 180); analogWrite(enB_F, 180);

  digitalWrite(in1_B, HIGH); digitalWrite(in2_B, LOW);
  digitalWrite(in3_B, LOW); digitalWrite(in4_B, HIGH);
  analogWrite(enA_B, 180); analogWrite(enB_B, 180);
}

void stopMotors() {
  int controlPins[] = {
    in1_F, in2_F, in3_F, in4_F,
    in1_B, in2_B, in3_B, in4_B
  };

  for (int i = 0; i < sizeof(controlPins) / sizeof(controlPins[0]); i++) {
    digitalWrite(controlPins[i], LOW);
  }

  analogWrite(enA_F, 0); analogWrite(enB_F, 0);
  analogWrite(enA_B, 0); analogWrite(enB_B, 0);
}
