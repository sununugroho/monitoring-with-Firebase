#include "DHT.h"
#include "FirebaseESP8266.h"

#define DHTPIN D4       // Pin DHT11 terhubung ke pin D5
#define DHTTYPE DHT11   // Jenis sensor DHT11
DHT dht(DHTPIN, DHTTYPE);

#define WIFI_SSID "sss"
#define WIFI_PASSWORD "999999999"

#define DATABASE_URL "https://monitoring-ed761-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "GSAIeNr9mUBVbKLmHON8NJMwAcyIgm4zdJx6HoSA"

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  Serial.begin(9600);
  dht.begin();

  connectWiFi();

  config.host = DATABASE_URL;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&config, &auth);
}

void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to WiFi, IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(1000);
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Kirim data suhu dan kelembaban ke Firebase
  if (Firebase.setFloat(firebaseData, "/sensor/temperature", temperature)) {
    Serial.println("Temperature sent to Firebase!");
  } else {
    Serial.println("Failed to send temperature to Firebase.");
    Serial.println("Reason: " + firebaseData.errorReason());
  }

  if (Firebase.setFloat(firebaseData, "/sensor/humidity", humidity)) {
    Serial.println("Humidity sent to Firebase!");
  } else {
    Serial.println("Failed to send humidity to Firebase.");
    Serial.println("Reason: " + firebaseData.errorReason());
  }

  delay(5000); // Kirim data setiap 5 detik
}