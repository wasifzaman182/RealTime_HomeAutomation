// Include the library files
#define BLYNK_PRINT Serial
#include <WiFi.h>               // ESP32 WiFi library
#include <BlynkSimpleEsp32.h>   // Blynk library for ESP32
#include <DHT.h>

// Blynk authentication and WiFi credentials
char auth[] = "LHD57jsUjB_QVPZdodvfB_FdmKccTB_b"; // Enter your Auth token
char ssid[] = "kmani";                             // Enter your WIFI name
char pass[] = "12345678";                          // Enter your WIFI password

// Initialize DHT sensor
DHT dht(15, DHT22); // (sensor pin, sensor type) - changed to DHT22
BlynkTimer timer;
bool pirbutton = 0;

// Define component pins
#define Buzzer 23   // GPIO23
#define MQ2 34      // ADC pin GPIO34
#define PIR 25      // GPIO25
#define relay1 22   // GPIO22
#define relay2 21   // GPIO21

// Get buttons values
BLYNK_WRITE(V0) {
  pirbutton = param.asInt();
}

void setup() {
  Serial.begin(115200); // Set baud rate for ESP32

  pinMode(Buzzer, OUTPUT);
  pinMode(PIR, INPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  dht.begin();

  // Call the functions
  timer.setInterval(100L, gassensor);
  timer.setInterval(100L, DHT22sensor);
  timer.setInterval(100L, pirsensor);
}

// Get the MQ2 sensor values
void gassensor() {
  int value = analogRead(MQ2);
  Serial.println(value);
  value = map(value, 0, 4095, 0, 100); // Adjusted for ESP32 ADC range
  if (value <= 55) {
    digitalWrite(Buzzer, LOW);
  } else if (value > 55) {
    Blynk.notify("Warning! Gas leak detected");
    digitalWrite(Buzzer, HIGH);
  }
  Blynk.virtualWrite(V1, value);
}

// Get the DHT22 sensor values
void DHT22sensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Blynk.virtualWrite(V2, t);
  Blynk.virtualWrite(V3, h);
}

// Get the PIR sensor values
void pirsensor() {
  bool value = digitalRead(PIR);
  if (pirbutton == 1) {
    if (value == 0) {
      digitalWrite(Buzzer, LOW);
    } else if (value == 1) {
      Blynk.notify("Warning! Please check your security system");
      digitalWrite(Buzzer, HIGH);
    }
  }
}

// Get buttons values
BLYNK_WRITE(V5) {
  bool RelayOne = param.asInt();
  if (RelayOne == 1) {
    digitalWrite(relay1, LOW);
  } else {
    digitalWrite(relay1, HIGH);
  }
}

// Get buttons values
BLYNK_WRITE(V6) {
  bool RelayTwo = param.asInt();
  if (RelayTwo == 1) {
    digitalWrite(relay2, LOW);
  } else {
    digitalWrite(relay2, HIGH);
  }
}

void loop() {
  Blynk.run(); // Run the Blynk library
  timer.run(); // Run the Blynk timer
}
