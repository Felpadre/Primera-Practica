#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Servidor Web
WebServer server(80);

// Variables de simulación
float temperatura = 0.0;
float humedad = 0.0;

// ===== FUNCIONES =====

// Simula o lee el DHT22 (en este caso simulamos valores)
void leerSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    temperatura = random(200, 320) / 10.0; // 20.0 - 32.0 °C
    humedad = random(300, 800) / 10.0;     // 30.0 - 80.0 %
  } else {
    temperatura = t;
    humedad = h;
  }
}

// Devuelve datos JSON (sin ArduinoJson)
void handleData() {
  leerSensor();
  String json = "{";
  json += "\"temp\":";
  json += String(temperatura, 1);
  json += ",\"hum\":";
  json += String(humedad, 1);
  json += "}";
  server.send(200, "application/json", json);
}

// Página principal simple
void handleRoot() {
  server.send(200, "text/plain", "Servidor ESP32 DHT22 con WiFiManager funcionando");
}

// ===== SETUP =====
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n===== INICIANDO ESP32 DHT22 (WiFiManager) =====");
  dht.begin();

  WiFiManager wm;

  // Si no se puede conectar, abre portal en modo AP
  bool res;
  res = wm.autoConnect("ESP32-DHT22-Setup", "12345678");

  if (!res) {
    Serial.println("⚠️ Falló la conexión o el timeout");
  } else {
    Serial.println("✅ Conectado correctamente!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  }

  // Configurar rutas del servidor
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
  Serial.println("Servidor Web iniciado ✅");
  Serial.println("Prueba en tu navegador: http://<IP_DEL_ESP32>/data");
}

// ===== LOOP =====
void loop() {
  server.handleClient();
}
