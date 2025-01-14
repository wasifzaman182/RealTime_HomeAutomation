// Include the necessary libraries for Blynk IoT
#define BLYNK_TEMPLATE_ID "TMPL4iJCjLX0H"
#define BLYNK_TEMPLATE_NAME "Home"
#include <BlynkSimpleEsp32.h>

// Blynk Authentication Token from the app
char auth[] = "zRP4JRUfkXA7iuVq75vI5hFQr9rIQFWg"; // Enter your Auth token

// Wi-Fi credentials
char ssid[] = "Oneplus1"; // Enter your Wi-Fi name
char pass[] = "asdfghjkl1"; // Enter your Wi-Fi password

// Define the pins
const int pirPin = 25;    // Pin for the PIR sensor (GPIO 25)
const int mq135Pin = 34;   // Pin for the MQ135 sensor (GPIO 34)
const int buzzerPin = 23;  // Pin for the buzzer (GPIO 23)
const int relayPin = 22;   // Pin for the relay (GPIO 22)

// Threshold for air quality (set to 100 for buzzing)
const int threshold = 55;  // Buzzer will turn on if the scaled value exceeds 55

// Variables to store sensor values
int motionDetected = LOW;  // Variable to store PIR sensor state
int mq135Value = 0;        // Variable to store MQ135 sensor value
int scaledGasValue = 0;    // Scaled gas value from 1 to 100
bool pirEnabled = false;   // Start with PIR sensor disabled (manual activation needed)

// Timer variables
unsigned long motionStartTime = 0;  // To store the time when motion was first detected
const unsigned long motionDuration = 10000;  // 10 seconds for the buzzer to stay on

// Notification flags
bool motionNotificationSent = false;  // To track if motion notification was sent
bool gasNotificationSent = false;    // To track if gas quality notification was sent

// Define virtual pins for Blynk
#define VIRTUAL_PIN_MOTION  V0  // Virtual Pin for PIR sensor
#define VIRTUAL_PIN_AIR_QUALITY  V1  // Virtual Pin for MQ135 sensor
#define VIRTUAL_PIN_PIR_CONTROL V5  // Virtual Pin for manual PIR control
#define VIRTUAL_PIN_RELAY_CONTROL V4 // Virtual Pin for Relay control

// Function to handle manual PIR control from the Blynk app
BLYNK_WRITE(VIRTUAL_PIN_PIR_CONTROL) {
  pirEnabled = param.asInt();  // Update the pirEnabled flag based on button state
  if (pirEnabled) {
    Serial.println("PIR sensor manually activated. Security system ON.");
  } else {
    Serial.println("PIR sensor manually deactivated. Security system OFF.");
    digitalWrite(buzzerPin, HIGH);  // Turn off the buzzer if PIR is disabled
    motionStartTime = 0;            // Reset motion detection timer
  }
}

// Function to handle relay control from the Blynk app
BLYNK_WRITE(VIRTUAL_PIN_RELAY_CONTROL) {
  int relayState = param.asInt(); // Get the state of the relay from the button (0 or 1)
  if (relayState == 1) {
    digitalWrite(relayPin, HIGH);  // Turn the relay ON
    Serial.println("Relay ON.");
  } else {
    digitalWrite(relayPin, LOW);   // Turn the relay OFF
    Serial.println("Relay OFF.");
  }
}

void setup() {
  // Initialize the PIR sensor pin as an input with internal pull-down resistor
  pinMode(pirPin, INPUT_PULLDOWN);  // Enable internal pull-down resistor

  // Initialize the MQ135 sensor pin as an input
  pinMode(mq135Pin, INPUT);

  // Initialize the buzzer pin as an output
  pinMode(buzzerPin, OUTPUT);

  // Initialize the relay pin as an output
  pinMode(relayPin, OUTPUT);

  // Ensure the buzzer is off initially (Active-low buzzer)
  digitalWrite(buzzerPin, HIGH);  // Active low: HIGH turns off the buzzer

  // Ensure the relay is off initially
  digitalWrite(relayPin, LOW);    // Relay is off initially

  // Start Serial Monitor for debugging
  Serial.begin(115200);

  // Inform the user that the system has started
  Serial.println("System started. PIR sensor is initially OFF. Activate manually via the Blynk app.");

  // Connect to Wi-Fi
  Blynk.begin(auth, ssid, pass);

  // Wait until connected to Blynk
  while (Blynk.connect() == false) {
    // Wait for the connection to Blynk server
    Serial.println("Connecting to Blynk...");
  }
  Serial.println("Connected to Blynk!");
}

void loop() {
  if (pirEnabled) {
    // Read the PIR sensor state
    motionDetected = digitalRead(pirPin);

    // Check if motion is detected
    if (motionDetected == HIGH) {  // Motion detected by PIR
      digitalWrite(buzzerPin, LOW);  // Turn buzzer ON (Active Low)
      Serial.println("Motion detected! Buzzer ON.");
      if (!motionNotificationSent) {  // Check if notification was already sent
        Blynk.logEvent("motion_detected");  // Trigger notification event
        motionNotificationSent = true;    // Set the flag to avoid duplicate notifications
      }
      // Start the timer when motion is detected
      if (motionStartTime == 0) {  // If the timer hasn't started yet
        motionStartTime = millis();  // Record the time when motion is first detected
      }
    } 
    // If no motion detected, turn off the buzzer
    else {
      digitalWrite(buzzerPin, HIGH);  // Turn buzzer OFF (Active Low)
      Serial.println("No motion. Buzzer OFF.");
      motionStartTime = 0;  // Reset the timer when no motion is detected
      motionNotificationSent = false;  // Reset the notification flag
    }

    // If the buzzer has been on for 10 seconds (motion detected for 10 seconds)
    if (motionStartTime > 0 && millis() - motionStartTime >= motionDuration) {
      digitalWrite(buzzerPin, HIGH);  // Turn buzzer OFF after 10 seconds (Active Low)
      Serial.println("Buzzer OFF after 10 seconds of motion.");
      motionStartTime = 0;  // Reset the timer
    }
  } else {
    motionDetected = LOW;  // Reset motion detection status when PIR is disabled
  }

  // Read the analog value from MQ135 sensor
  mq135Value = analogRead(mq135Pin);

  // Scale the MQ135 value from 100-1023 to 1-100
  scaledGasValue = map(mq135Value, 100, 1023, 0, 100);

  // Clamp the scaledGasValue to ensure it's between 0 and 100
  scaledGasValue = constrain(scaledGasValue, 0, 100);

  // Debugging: Print the MQ135 sensor value and scaled gas value to the Serial Monitor
  Serial.print("MQ135 Value: ");
  Serial.println(mq135Value);

  Serial.print("Scaled Gas Value: ");
  Serial.println(scaledGasValue);

  // Check if air quality (scaled gas value) exceeds the threshold
  if (scaledGasValue > threshold) {  // Scaled gas value is poor (value > threshold)
    digitalWrite(buzzerPin, LOW);  // Turn buzzer ON (Active Low)
    Serial.println("Air quality is poor! Buzzer ON.");
    if (!gasNotificationSent) {  // Check if notification was already sent
      Blynk.logEvent("gas_leak_detected");  // Trigger notification event
      gasNotificationSent = true;  // Set the flag to avoid duplicate notifications
    }
  } 
  // If air quality is acceptable, turn off the buzzer
  else if (motionStartTime == 0) {  // Only turn off buzzer if no motion was detected
    digitalWrite(buzzerPin, HIGH);  // Turn buzzer OFF (Active Low)
    Serial.println("Air quality is acceptable. Buzzer OFF.");
    gasNotificationSent = false;  // Reset the notification flag
  }

  // Send data to Blynk app
  Blynk.virtualWrite(VIRTUAL_PIN_MOTION, motionDetected);  // Send motion detected status to Blynk app
  Blynk.virtualWrite(VIRTUAL_PIN_AIR_QUALITY, scaledGasValue);  // Send scaled gas value to Blynk app

  // Add delay to ensure PIR sensor stabilizes and to debounce it
  delay(2000);

  // Ensure Blynk runs in the loop
  Blynk.run();
}
