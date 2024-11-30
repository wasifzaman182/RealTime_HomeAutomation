// Include the library files
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL4t8N2Vcv1"
#define BLYNK_TEMPLATE_NAME "Home Automation"
#define BLYNK_AUTH_TOKEN "tfRM-prCXr8cKXZBAG4M65mSKf-YGJ0v"
#include <WiFi.h>               // ESP32 WiFi library
#include <BlynkSimpleEsp32.h>   // Blynk library for ESP32
#include <DHT.h>

// Blynk authentication and WiFi credentials
char auth[] = "tfRM-prCXr8cKXZBAG4M65mSKf-YGJ0v"; // Enter your Auth token
char ssid[] = "Oneplus";                             // Enter your WIFI name
char pass[] = "asdfghjkl";                          // Enter your WIFI password

// Initialize DHT sensor
DHT dht(15, DHT22); // (sensor pin, sensor type)
BlynkTimer timer;
bool pirbutton = 0;

// Define component pins
#define Buzzer 23   // GPIO23
//#define MQ135 34    // ADC pin GPIO34
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
//  timer.setInterval(1000L, gassensor); // Adjust to 1s interval
  timer.setInterval(1000L, DHT22sensor);
  timer.setInterval(100L, pirsensor);
}
/*
void gassensor() {
  int rawValue = analogRead(MQ135); // Read raw analog value
  Serial.println(rawValue);

  // Map raw value to a percentage (adjust the range based on your environment)
  int gasLevel = map(rawValue, 0, 4095, 0, 100); // ESP32 ADC range is 0-4095

  // Determine air quality levels (adjust thresholds based on your use case)
  if (gasLevel < 30) {
    digitalWrite(Buzzer, LOW); // Good air quality, buzzer OFF
  } else if (gasLevel >= 30 && gasLevel <= 70) {
    Serial.println(gasLevel);
    Serial.println("Warning! Air quality deteriorating.");
    //digitalWrite(Buzzer, LOW); // Moderate air quality, buzzer remains OFF
  } else if (gasLevel > 70) {
    Blynk.logEvent("Critical air quality detected!");
    digitalWrite(Buzzer, HIGH); // Poor air quality, buzzer ON
  }

  // Send the gas level to Blynk
  Blynk.virtualWrite(V1, gasLevel);
}
*/

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
      Blynk.logEvent("Warning! Please check your security system");
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
