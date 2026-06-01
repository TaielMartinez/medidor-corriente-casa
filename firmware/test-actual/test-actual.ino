/**
 * Test WiFi + corriente instantánea — ESP32-C3 SuperMini
 *
 * - WiFi + servidor HTTP :80 (igual que test-wifi)
 * - SCT-013 Casa 1 en GPIO0 (ADC1 channel 0) vía EmonLib
 * - GET /actual/1 → lectura instantánea en A (JSON)
 *
 * Placa: ESP32C3 Dev Module, USB CDC On Boot: Enabled
 */

#include <WiFi.h>
#include <WebServer.h>
#include "EmonLib.h"

#include "secrets.h"

// Casa 1 — GPIO0 = ADC1_CH0 en ESP32-C3
#define ADC_CASA1     0
#define CAL_CASA1     30
#define V_RED         220.0f
#define ADC_BITS      10
#define MEASURE_MS    1000
#define CALC_IRMS_SAMPLES 1480

WebServer server(80);
EnergyMonitor emon1;

float iRmsCasa1 = 0.0f;
unsigned long lastMeasure = 0;

void sendCorsHeaders() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

void sendJson(int code, const char* body) {
  sendCorsHeaders();
  server.send(code, "application/json", body);
}

void handleCorsPreflight() {
  sendCorsHeaders();
  server.send(204);
}

void respondTrue() {
  sendJson(200, "true");
}

void setupEmon() {
  analogReadResolution(ADC_BITS);
#if defined(ESP32)
  analogSetPinAttenuation(ADC_CASA1, ADC_11db);
#endif
  emon1.current(ADC_CASA1, CAL_CASA1);
}

void measureCasa1() {
  iRmsCasa1 = emon1.calcIrms(CALC_IRMS_SAMPLES);
}

void handleActual1() {
  char body[96];
  snprintf(body, sizeof(body),
           "{\"casa\":1,\"i_rms_a\":%.3f,\"p_w\":%.1f}",
           iRmsCasa1, iRmsCasa1 * V_RED);
  sendJson(200, body);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("test-actual ESP32-C3");

  setupEmon();

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

  server.on("/", HTTP_GET, respondTrue);
  server.on("/", HTTP_OPTIONS, handleCorsPreflight);
  server.on("/api/health", HTTP_GET, respondTrue);
  server.on("/api/health", HTTP_OPTIONS, handleCorsPreflight);
  server.on("/actual/1", HTTP_GET, handleActual1);
  server.on("/actual/1", HTTP_OPTIONS, handleCorsPreflight);
  server.onNotFound([]() {
    if (server.method() == HTTP_OPTIONS) {
      handleCorsPreflight();
      return;
    }
    sendJson(404, "false");
  });
  server.begin();

  lastMeasure = millis();
  measureCasa1();
  Serial.printf("Primera lectura casa1: %.3f A\n", iRmsCasa1);
  Serial.println("HTTP :80 — GET /actual/1");
}

void loop() {
  unsigned long now = millis();
  if (now - lastMeasure >= MEASURE_MS) {
    lastMeasure = now;
    measureCasa1();
    Serial.printf("I_rms casa1: %.3f A\n", iRmsCasa1);
  }
  server.handleClient();
}
