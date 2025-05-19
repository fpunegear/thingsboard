#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <ArduinoJson.h>

const char* id = "ASD123";
const char* side = "right";

const int numSensors = 8;
float listPositionsXY[numSensors][2] = {
    {4, 18.3}, {1.1, 19.5}, {4.7, 14.7}, {0.5, 13.8},
    {4.7, 9}, {1.3, 7.5}, {3, 0}, {1.5, 0}
};

const char* ssid = "mywifi";
const char* password = "supersecret";

const char* mqtt_server = "192.168.0.8";
const char* mqtt_topic = "v1/devices/me/telemetry";
const char* mqtt_client_id = "sole101";
const char* mqtt_user = "tenant@thingsboard.org";
const char* mqtt_password = "tenant";


WiFiClient wifi_client;
PubSubClient client(wifi_client);

long lastMsg = 0;

int s0 = A0;
int s1 = A1;
int s2 = A2;
int s3 = A3;

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
  if (now - lastMsg > 1000) {
    lastMsg = now;

    StaticJsonDocument<1024> payload;

    payload["id"] = id;
    payload["sd"] = side;
    String sensorData = ""; // Initialize a separate String to hold sensor data

    for (int i = 0; i < numSensors; i++) {
      float weight = readMux(i);
      float posX = listPositionsXY[i][0];
      float posY = listPositionsXY[i][1];
      
      // Append sensor data to the sensorData string
      sensorData += String(posX) + "_" + String(posY) + "_" + String(weight) + ",";

      Serial.printf("Sensor %d: Weight = %.2f, Position = [%.2f, %.2f]\n", i, weight, posX, posY);
    }



    // Assign the built string to the payload
    if (sensorData.length() > 0) {
      sensorData.remove(sensorData.length() - 1); // Remove the last comma
    }
    payload["vl"] = sensorData; // Assign the final string to the payload

    String jsonString;
    serializeJson(payload, jsonString);

    Serial.println("[JSON] Sending data:");
    Serial.println(jsonString);

    if (client.publish(mqtt_topic, jsonString.c_str())) {
      Serial.println("[MQTT] Message sent successfully.");
    } else {
      Serial.println("[MQTT] Message failed to send.");
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

  for (int i = 0; i < 4; i++) {
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }

  int val = analogRead(SIG_pin);
  float voltage = val;
  // float voltage = (val * 5.0) / 1024.0;
  return voltage;
}
