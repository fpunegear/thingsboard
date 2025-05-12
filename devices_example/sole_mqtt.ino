#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <ArduinoJson.h>

// Sole and sensor hardcoded data
const char* id = "ASD123";
const char* side = "right";

const int numSensors = 8; // Using channels 0–7
float listPositionsXY[numSensors][2] = {
    {12.2, 10.3}, {12.5, 11.3}, {13.0, 12.1}, {14.4, 13.5},
    {15.2, 14.8}, {16.1, 15.9}, {17.3, 16.4}, {18.0, 17.2}
};

// Replace with your WiFi credentials
const char* ssid = "mywifi";
const char* password = "supersecret";

// MQTT Broker settings
const char* mqtt_server = "192.168.0.8";
const char* mqtt_topic = "v1/devices/me/telemetry";
const char* mqtt_client_id = "sole101";
const char* mqtt_user = "tenant@thingsboard.org"; // User
const char* mqtt_password = "tenant"; // Password

WiFiClient wifi_client;
PubSubClient client(wifi_client);

long lastMsg = 0;

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

  client.setServer(mqtt_server, 1883);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(mqtt_client_id, mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
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

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;

    // Create JSON object
    StaticJsonDocument<1024> res; // Increased size to handle larger JSON
    res["id"] = id;
    res["sd"] = side;

    // Add sensor values to JSON
    JsonArray values = res.createNestedArray("vl");

    for (int i = 0; i < numSensors; i++) {
      JsonObject sensorValue = values.createNestedObject();
      sensorValue["w"] = readMux(i);

      JsonArray positions = sensorValue.createNestedArray("p");
      positions.add(listPositionsXY[i][0]); // X position
      positions.add(listPositionsXY[i][1]); // Y position

      Serial.printf("Sensor %d: Weight = %.2f, Position = [%.2f, %.2f]\n", 
                    i, readMux(i), listPositionsXY[i][0], listPositionsXY[i][1]);
    }

    // Serialize JSON to string
    String jsonString;
    // String jsonString = "{\"id\":\"ASD123\",\"side\":\"right\",\"values\":[{\"weight\":0,\"position\":[12.2,10.3]},{\"weight\":0,\"position\":[12.5,11.3]},{\"weight\":0,\"position\":[13,12.1]},{\"weight\":0,\"position\":[14.4,13.5]},{\"weight\":0,\"position\":[15.2,14.8]}]}";
    serializeJson(res, jsonString);

    // Debug: Print JSON string
    Serial.println("[JSON] Sending data:");
    Serial.println(jsonString);
    Serial.println(jsonString.c_str());

    // Publish the message (publish fail if msg is too long)
    if (client.publish(mqtt_topic, jsonString.c_str())) {
      Serial.println("Message sent successfully.");
    } else {
      Serial.println("Message failed to send.");
      Serial.println(client.state());
    }
  }
}

float readMux(int channel) {
  int controlPin[] = {s0, s1, s2, s3};

  int muxChannel[numSensors][4] = {
    {0, 0, 0, 0}, {1, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 0, 0},
    {0, 0, 1, 0}, {1, 0, 1, 0}, {0, 1, 1, 0}, {1, 1, 1, 0}
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
