#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Sole and sensor hardcoded data
const char* id = "ASD123";
const char* side = "right";
float listPositionsXY[16][2] = {
    {12.2, 10.3}, {12.5, 11.3}, {13.0, 12.1}, {14.4, 13.5},
    {15.2, 14.8}, {16.1, 15.9}, {17.3, 16.4}, {18.0, 17.2},
    {19.5, 18.6}, {20.1, 19.3}, {21.0, 20.0}, {22.2, 21.1},
    {23.3, 22.4}, {24.5, 23.6}, {25.7, 24.8}, {26.9, 25.9}
};

// WiFi credentials
const char* ssid = "mywifi";
const char* password = "supersecret";

// Server URL
const char* serviceUrl = "http://thingsboard.semppy.co:8080/api/v1/T2_TEST_TOKEN/telemetry";

// Mux control pins
int s0 = A0;
int s1 = A1;
int s2 = A2;
int s3 = A3;

// Mux SIG pin
int SIG_pin = 4;

void setup() {
  pinMode(s0, OUTPUT); 
  pinMode(s1, OUTPUT); 
  pinMode(s2, OUTPUT); 
  pinMode(s3, OUTPUT); 

  digitalWrite(s0, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);

  Serial.begin(115200);
  Serial.setDebugOutput(true);

  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("[SETUP] Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\n[SETUP] WiFi connected");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WiFi] Disconnected! Reconnecting...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }
    Serial.println("\n[WiFi] Reconnected!");
  }

  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    if (!http.begin(client, serviceUrl)) {
      Serial.println("[HTTP] Unable to connect");
      return;
    }
    http.addHeader("Content-Type", "application/json");

    // Create JSON object
    StaticJsonDocument<1024> res; // Increased size to handle larger JSON
    res["id"] = id;
    res["side"] = side;

    // Add sensor values to JSON
    JsonArray values = res.createNestedArray("values");

    for (int i = 0; i < 16; i++) {
      JsonObject sensorValue = values.createNestedObject();
      sensorValue["weight"] = readMux(i);

      JsonArray positions = sensorValue.createNestedArray("position");
      positions.add(listPositionsXY[i][0]); // X position
      positions.add(listPositionsXY[i][1]); // Y position

      Serial.printf("Sensor %d: Weight = %.2f, Position = [%.2f, %.2f]\n", 
                    i, readMux(i), listPositionsXY[i][0], listPositionsXY[i][1]);
    }

    // Serialize JSON to string
    String jsonString;
    serializeJson(res, jsonString);

    // Debug: Print JSON string
    Serial.println("[JSON] Sending data:");
    Serial.println(jsonString);

    // Send HTTP POST request
    int httpResponseCode = http.POST(jsonString);

    if (httpResponseCode > 0) {
      Serial.printf("[HTTP] Response code: %d\n", httpResponseCode);
      String response = http.getString();
      Serial.println("[HTTP] Response:");
      Serial.println(response);
    } else {
      Serial.printf("[HTTP] POST failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
    }

    http.end();
  }

  delay(2000); // Delay between requests
}

float readMux(int channel) {
  int controlPin[] = {s0, s1, s2, s3};

  int muxChannel[16][4] = {
    {0, 0, 0, 0}, {1, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 0, 0},
    {0, 0, 1, 0}, {1, 0, 1, 0}, {0, 1, 1, 0}, {1, 1, 1, 0},
    {0, 0, 0, 1}, {1, 0, 0, 1}, {0, 1, 0, 1}, {1, 1, 0, 1},
    {0, 0, 1, 1}, {1, 0, 1, 1}, {0, 1, 1, 1}, {1, 1, 1, 1}
  };

  // Set control pins
  for (int i = 0; i < 4; i++) {
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }

  // Read the value at the SIG pin
  int val = analogRead(SIG_pin);

  // Convert to voltage (optional)
  float voltage = val; // (val * 5.0) / 1024.0;
  return voltage;
}
