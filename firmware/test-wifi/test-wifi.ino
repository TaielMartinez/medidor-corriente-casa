/**
 * Test WiFi + HTTP — ESP32-C3 SuperMini
 *
 * Conecta a la red y expone GET / → true (JSON) en el puerto 80.
 *
 * Arduino IDE:
 *   Placa: ESP32C3 Dev Module
 *   USB CDC On Boot: Enabled (si no ves Serial)
 *   Partition: Default
 */

#include <WiFi.h>
#include <WebServer.h>

#include "secrets.h"

WebServer server(80);

void respondTrue() {
  server.send(200, "application/json", "true");
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("test-wifi ESP32-C3");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.printf("Conectando a \"%s\"...\n", WIFI_SSID);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - start > 30000) {
      Serial.println("ERROR: timeout WiFi (30 s)");
      return;
    }
    delay(500);
    Serial.print('.');
  }
  Serial.println();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("RSSI: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");

  server.on("/", respondTrue);
  server.on("/api/health", respondTrue);
  server.onNotFound([]() {
    server.send(404, "application/json", "false");
  });
  server.begin();
  Serial.println("HTTP :80 listo — GET / o GET /api/health");
}

void loop() {
  server.handleClient();
}
