#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

// Reemplaza las siguientes variables con tu combinación de SSID/Contraseña
const char* ssid = "mywifi";
const char* password = "supersecret";

// Dirección IP de tu broker MQTT
const char* mqtt_server = "192.168.0.8";
const char* mqtt_topic = "v1/devices/me/telemetry";
const char* mqtt_client_id = "sole101";
const char* mqtt_user = "tenant@thingsboard.org"; // Usuario
const char* mqtt_password = "tenant"; // Contraseña

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;

void setup() {
  Serial.begin(115200);
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
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
  Serial.println("WiFi conectado");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Mensaje recibido en el tema: ");
  Serial.print(topic);
  Serial.print(". Mensaje: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT...");
    if (client.connect(mqtt_client_id, mqtt_user, mqtt_password)) {
      Serial.println("conectado");
    } else {
      Serial.print("falló, rc=");
      Serial.print(client.state());
      Serial.println(" intenta de nuevo en 5 segundos");
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
    
    // Temperatura en Celsius
    float temperature = 25;

    // Crear el mensaje JSON
    String payload = String("{\"temperature\":") + temperature + "}";
    Serial.print("Enviando: ");
    Serial.println(payload);
    
    // Publicar el mensaje
    client.publish(mqtt_topic, payload.c_str());
  }
}
