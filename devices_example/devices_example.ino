#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>

#define USE_SERIAL Serial

WiFiMulti wifiMulti;

void setup() {

  USE_SERIAL.begin(115200);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }


  wifiMulti.addAP("ssid_name", "wifi_pass");

}

void loop() {
  // wait for WiFi connection
  if ((wifiMulti.run() == WL_CONNECTED)) {

    HTTPClient http;

    USE_SERIAL.print("[HTTP] begin...\n");
    // configure traged server and url

    // http.begin("http://192.168.0.8:8080/api/v1/MNdiqtN9kD87fhNYQfiG/telemetry");
    http.begin("http://[here your ip]:8080/api/v1/MNdiqtN9kD87fhNYQfiG/telemetry");

    http.addHeader("Content-Type", "application/json");

    // Define el cuerpo del POST en formato JSON
    String jsonPayload = "{\"id\":\"ASD123\",\"side\":\"right\",\"values\":[{\"weight\":0.5,\"position\":25},{\"weight\":0.6,\"position\":23},{\"weight\":0.8,\"position\":12},{\"weight\":0.1,\"position\":26},{\"weight\":0.5,\"position\":12},{\"weight\":0.5,\"position\":10}]}";

    USE_SERIAL.print("[HTTP] POST...\n");
    // start connection and send HTTP header
    int httpCode = http.POST(jsonPayload);

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        USE_SERIAL.println(payload);
      }
    } else {
      USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }

  delay(10000);
}
