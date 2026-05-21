// --- LIBRARIES (MUST BE AT THE TOP) ---
#define BLYNK_PRINT Serial

// --- BLYNK CREDENTIALS (REQUIRED) ---
#define BLYNK_TEMPLATE_ID   "TMPL3MXuyifwP"
#define BLYNK_TEMPLATE_NAME "Warehouse Monitoring System"
#define BLYNK_AUTH_TOKEN    "o90z9msrShx0CytPuaktdsdJDfFabf-g"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "DHT.h"
// --- WIFI DETAILS ---
char ssid[] = "vivo Y28 5G";
char pass[] = "muruga@123";

// --- PIN DEFINITIONS ---
#define DHTPIN   D4
#define DHTTYPE  DHT11
#define TRIG_PIN D5
#define ECHO_PIN D6

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// ---------- FLOAT TO BINARY FUNCTION ----------
String floatToBinary(float value) {
  String binaryString = "";
  byte* b = (byte*)&value;
  for (int i = 3; i >= 0; i--) {
    String part = String(b[i], BIN);
    while (part.length() < 8) part = "0" + part;
    binaryString += part;
  }
  return binaryString;
}

// ---------- SEND SENSOR DATA ----------
void sendSensorData() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int gasValue = analogRead(A0);

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  float distanceCm = duration * 0.034 / 2;

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // ---- Convert to Binary ----
  String tempBinary = floatToBinary(t);
  String humBinary  = floatToBinary(h);
  String gasBinary  = String(gasValue, BIN);
  String distBinary = floatToBinary(distanceCm);

  // ---- Serial Output ----
  Serial.println("==============================");
  Serial.print("Temperature: "); Serial.print(t); Serial.print(" | Binary: ");
  Serial.println(tempBinary);
  Serial.print("Humidity: "); Serial.print(h); Serial.print(" | Binary: ");
  Serial.println(humBinary);
  Serial.print("Gas Level: "); Serial.print(gasValue); Serial.print(" | Binary: ");
  Serial.println(gasBinary);
  Serial.print("Distance: "); Serial.print(distanceCm); Serial.print(" cm | Binary: ");
  Serial.println(distBinary);
  Serial.println("==============================");

  // ---- Blynk Upload ----
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V2, gasValue);
  Blynk.virtualWrite(V3, distanceCm);

  Blynk.virtualWrite(V10, tempBinary);
  Blynk.virtualWrite(V11, humBinary);
  Blynk.virtualWrite(V12, gasBinary);
  Blynk.virtualWrite(V13, distBinary);
}

// ---------- SETUP ----------
void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(5000L, sendSensorData);

  Serial.println("Smart Warehouse System Started...");
}

// ---------- MAIN LOOP ----------
void loop() {
  Blynk.run();
  timer.run();
}

