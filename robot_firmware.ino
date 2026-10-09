/**
 * @file robot_firmware.ino
 * @brief Multi-Purpose Water Spraying Robot with Wi-Fi Telemetry & Emergency Mode
 * @author Atharv Anil Jadhav
 */

#include <WiFi.h>
#include <WiFiClient.h>

// Wi-Fi Credentials for AP/STA mode
const char* ssid = "AgriRobot_AP";
const char* password = "Password123";

// Motor Driver Pins (L298N)
#define IN1 26
#define IN2 27
#define IN3 14
#define IN4 12
#define ENA 32
#define ENB 33

// Water Pump & Flame/Emergency Pins
#define PUMP_RELAY_PIN 25
#define FLAME_SENSOR_PIN 34
#define STATUS_LED 2

WiFiServer server(80);

void setup() {
  Serial.begin(115200);

  // Configure Motor & Actuator Outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  pinMode(FLAME_SENSOR_PIN, INPUT);

  // Turn off relay & motors by default
  digitalWrite(PUMP_RELAY_PIN, LOW);
  stopMotors();

  // Create Access Point for Mobile Control
  WiFi.softAP(ssid, password);
  Serial.print("Access Point Created. IP: ");
  Serial.println(WiFi.softAPIP());

  server.begin();
}

void loop() {
  // Emergency Detection: Automatic override if fire/smoke detected
  checkEmergencyConditions();

  // Client Command Handling
  WiFiClient client = server.available();
  if (client) {
    String command = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        command += c;
        if (c == '\n') break;
      }
    }
    executeCommand(command);
  }
}

void executeCommand(String cmd) {
  cmd.trim();
  if (cmd == "F") moveForward();
  else if (cmd == "B") moveBackward();
  else if (cmd == "L") turnLeft();
  else if (cmd == "R") turnRight();
  else if (cmd == "S") stopMotors();
  else if (cmd == "PUMP_ON") digitalWrite(PUMP_RELAY_PIN, HIGH);
  else if (cmd == "PUMP_OFF") digitalWrite(PUMP_RELAY_PIN, LOW);
}

void checkEmergencyConditions() {
  int flameStatus = digitalRead(FLAME_SENSOR_PIN);
  if (flameStatus == LOW) { // Flame detected
    digitalWrite(STATUS_LED, HIGH);
    digitalWrite(PUMP_RELAY_PIN, HIGH); // Auto emergency water dispersion
    stopMotors();
    Serial.println("EMERGENCY OVERRIDE: Flame Detected!");
  } else {
    digitalWrite(STATUS_LED, LOW);
  }
}

void moveForward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void moveBackward() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}

void turnLeft() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void turnRight() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}

void stopMotors() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}
