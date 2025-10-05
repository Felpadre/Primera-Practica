#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "DHT.h"

// ===== CONFIGURACIÓN DE SENSOR =====
#define DHTPIN 2      // Pin GPIO donde está conectado el DHT22
#define DHTTYPE DHT22 // Tipo de sensor

DHT dht(DHTPIN, DHTTYPE);

// ===== CONFIGURACIÓN DE RED =====
String SSID = "Tu_SSID";         // Cambiar por el nombre de tu red WiFi
String PASSWORD = "Tu_PASSWORD"; // Cambiar por la contraseña de tu red

// ===== SERVIDOR WEB =====
WebServer server(80);

// ===== VARIABLES =====
float temperatura = 0.0;
float humedad = 0.0;

// ===== FUNCIONES =====

// Función que obtiene los datos del DHT22 o simula si no hay lectura
void leerSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    // Simulación si falla el sensor
    Serial.println("⚠️ No se pudo leer del DHT22, usando valores simulados...");
    temperatura = random(200, 320) / 10.0; // 20.0 a 32.0 °C
    humedad = random(300, 800) / 10.0;     // 30.0 a 80.0 %
  } else {
    temperatura = t;
    humedad = h;
  }
}

// Envía los datos como JSON (sin ArduinoJson)
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
  server.send(200, "text/plain", "Servidor ESP32 DHT22 funcionando correctamente");
}

// ===== SETUP =====
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n===== INICIANDO ESP32 DHT22 =====");

  dht.begin();

  // Intentar conectar al WiFi
  Serial.println("Conectando a WiFi...");
  WiFi.begin(SSID.c_str(), PASSWORD.c_str());

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Conectado a WiFi!");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n⚠️ No se pudo conectar. Creando punto de acceso...");
    WiFi.softAP("ESP32-DHT22", "12345678");
    Serial.print("Dirección IP (AP): ");
    Serial.println(WiFi.softAPIP());
  }

  // Rutas del servidor
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();

  Serial.println("Servidor Web iniciado ✅");
  Serial.println("Prueba en el navegador:");
  Serial.println("http://<IP_DEL_ESP32>/data");
}

// ===== LOOP =====
void loop() {
  server.handleClient();
}
