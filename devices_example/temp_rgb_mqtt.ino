#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
// #include <analogWrite.h>  // Librería para usar analogWrite en ESP32

// ---------------------- CONFIG WIFI ----------------------
const char* ssid = "ssi_example";
const char* password = "password_example";

// ---------------------- CONFIG MQTT ----------------------
const char* mqtt_server   = "x.x.x.x"; // direccion del server
const char* mqtt_topic    = "v1/devices/me/telemetry";
const char* mqtt_client_id = "dht11-sensor";
const char* mqtt_user     = "dht11-sensor"; 
const char* mqtt_password = "dht11-sensor";

// {clientId:"dht11-sensor",userName:"dht11-sensor",password:"dht11-sensor"}

WiFiClient espClient;
PubSubClient client(espClient);

// ---------------------- CONFIG DHT11 ----------------------
#define PIN_DHT 7
#define DHTTYPE DHT11
DHT dht(PIN_DHT, DHTTYPE);

// ---------------------- CONFIG LED RGB ----------------------
#define PIN_R 2   // Ajusta según tu conexión
#define PIN_G 3
#define PIN_B 4

// ---------------------- VARIABLES ----------------------
long lastMsg = 0;

void setup() {
  Serial.begin(115200);

  // WiFi
  setup_wifi();

  // MQTT
  client.setServer(mqtt_server, 1883);

  // DHT11
  dht.begin();

  // LED RGB
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("✅ WiFi conectado");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT...");
    if (client.connect(mqtt_client_id, mqtt_user, mqtt_password)) {
      Serial.println(" conectado ✅");
    } else {
      Serial.print(" fallo, rc=");
      Serial.print(client.state());
      Serial.println(" → reintentando en 5s");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;

    float temp = dht.readTemperature();

    if (isnan(temp)) {
      Serial.println("❌ Error al leer el DHT11");
      return;
    }

    Serial.print("🌡 Temperatura: ");
    Serial.print(temp);
    Serial.println(" °C");

    // --- Escalado Azul (25°C) → Lila (30°C) → Rojo (35°C) ---
    if (temp < 25) temp = 25;
    if (temp > 35) temp = 35;

    int val = map(temp * 10, 250, 350, 0, 255);
    int r = val;        // sube con temperatura
    int g = 0;          // fijo
    int b = 255 - val;  // baja con temperatura

    analogWrite(PIN_R, r);
    analogWrite(PIN_G, g);
    analogWrite(PIN_B, b);

    Serial.print("LED -> R:");
    Serial.print(r);
    Serial.print(" G:");
    Serial.print(g);
    Serial.print(" B:");
    Serial.println(b);

    // --- Publicar en MQTT ---
    String payload = String("{\"temperature\":") + temp + "}";
    Serial.print("📤 Enviando MQTT: ");
    Serial.println(payload);

    client.publish(mqtt_topic, payload.c_str());
  }
}
