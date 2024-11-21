#include <WiFi.h> // Replace ESP8266WiFi.h with WiFi.h for ESP32
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// Wi-Fi credentials
const char* ssid = "TP-LINK_AP_CC4A";
const char* password = "";

// MQTT broker credentials
#define AIO_SERVER      "broker.hivemq.com"
#define AIO_SERVERPORT  1883 // use 8883 for SSL
#define AIO_USERNAME    "adesh"
#define AIO_KEY         "123"

// Create a WiFi client for the MQTT connection
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// MQTT topics
Adafruit_MQTT_Publish led1status = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/led1stat");
Adafruit_MQTT_Publish led2status = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/led2stat");
Adafruit_MQTT_Publish relaystatus = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/relaystat");

Adafruit_MQTT_Subscribe led1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/led1");
Adafruit_MQTT_Subscribe led2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/led2");
Adafruit_MQTT_Subscribe relay = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/relay");

void MQTT_connect();

void setup() {
    // Initialize GPIO pins for output
    pinMode(16, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(0, OUTPUT);

    Serial.begin(115200);
    Serial.println("\nMQTT Demo");
    
    // Connect to Wi-Fi
    Serial.print("Connecting to SSID: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    delay(500);

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        digitalWrite(16, HIGH);
        delay(100);
        digitalWrite(16, LOW);
        delay(100);
    }

    Serial.println("\nWiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Subscribe to MQTT topics
    mqtt.subscribe(&led1);
    mqtt.subscribe(&led2);
    mqtt.subscribe(&relay);
}

void MQTT_connect() {
    int8_t ret;
    if (mqtt.connected()) {
        return;
    }

    Serial.print("Connecting to MQTT... ");
    uint8_t retries = 5;

    while ((ret = mqtt.connect()) != 0) {
        Serial.println(mqtt.connectErrorString(ret));
        Serial.println("Retrying MQTT connection in 5 seconds...");
        mqtt.disconnect();
        delay(5000);
        retries--;
        if (retries == 0) {
            while (1);
        }
    }

    Serial.println("MQTT Connected!");
    for (int i = 0; i < 3; i++) {
        digitalWrite(4, HIGH);
        delay(100);
        digitalWrite(4, LOW);
        delay(100);
    }
}

void loop() {
    // Maintain MQTT connection
    MQTT_connect();

    // Handle MQTT subscriptions
    Adafruit_MQTT_Subscribe *subscription;
    while ((subscription = mqtt.readSubscription(5000))) {
        if (subscription == &led1) {
            Serial.print(F("LED1: "));
            Serial.println((char *)led1.lastread);
            String ledStatus1 = String((char *)led1.lastread);
            if (ledStatus1 == "ON") {
                digitalWrite(4, HIGH);
                led1status.publish("LED 1 TURNED ON");
            } else if (ledStatus1 == "OFF") {
                digitalWrite(4, LOW);
                led1status.publish("LED 1 TURNED OFF");
            }
        }

        if (subscription == &led2) {
            Serial.print(F("LED2: "));
            Serial.println((char *)led2.lastread);
            String ledStatus2 = String((char *)led2.lastread);
            if (ledStatus2 == "ON") {
                digitalWrite(16, HIGH);
                led2status.publish("LED 2 TURNED ON");
            } else if (ledStatus2 == "OFF") {
                digitalWrite(16, LOW);
                led2status.publish("LED 2 TURNED OFF");
            }
        }

        if (subscription == &relay) {
            Serial.print(F("RELAY: "));
            Serial.println((char *)relay.lastread);
            String relayStatus = String((char *)relay.lastread);
            if (relayStatus == "ON") {
                digitalWrite(0, LOW);
                relaystatus.publish("RELAY TURNED ON");
            } else if (relayStatus == "OFF") {
                digitalWrite(0, HIGH);
                relaystatus.publish("RELAY TURNED OFF");
            }
        }
    }
}
