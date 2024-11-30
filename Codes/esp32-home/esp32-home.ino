// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "TMPL4t8N2Vcv1"
#define BLYNK_TEMPLATE_NAME "Home Automation"
#define BLYNK_AUTH_TOKEN "tfRM-prCXr8cKXZBAG4M65mSKf-YGJ0v"
//------------------------------------------------------------------------------
//--------------------------------------------------
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

char auth[] = "tfRM-prCXr8cKXZBAG4M65mSKf-YGJ0v";
BlynkTimer timer;
//--------------------------------------------------
// Your WiFi Router's credentials. (or Mobile Hotspot)
// Set password to "" for open networks.
char ssid[] = "Oneplus";
char pass[] = "asdfghjkl";

bool pirbutton = 0;
//--------------------------------------------------
/*#define BUTTON_1 22 
#define BUTTON_2 15
#define BUTTON_3 4
#define BUTTON_4 16
//--------------------------------------------------
#define RELAY_1 27
#define RELAY_2 26
#define RELAY_3 25
#define RELAY_4 33*/
//--------------------------------------------------
#define BUZZER  23
//#define TRIG    12
//#define ECHO    13
#define PIR     25 //pir security button pin
#define MQ2     34
#define relay1 22   // GPIO22
#define relay2 21   // GPIO21
#define DHT_PIN 15
//#define FLAME   21
//--------------------------------------------------
/*int STATE_RELAY_1 = 0;
int STATE_RELAY_2 = 0;
int STATE_RELAY_3 = 0;
int STATE_RELAY_4 = 0;*/
//--------------------------------------------------
BLYNK_WRITE(V0) {
  pirbutton = param.asInt();
}
//Change the virtual pins according the rooms
//#define VPIN_BUTTON_1     V0
//#define VPIN_PIR          V4
#define VPIN_TEMPERATURE  V2
#define VPIN_HUMIDITY     V3
//#define VPIN_ULTRASONIC   V7
#define VPIN_MQ2          V1
//#define VPIN_FLAME        V9
//------------------------------------------------------
boolean pir_new     = LOW;  // current state of pin
boolean pir_old     = LOW;  // previous state of pin

boolean flame_new   = LOW;  // current state of pin
boolean flame_old   = LOW;  // previous state of pin

int mq2_new         = LOW;  // current state of pin
int mq2_old         = LOW;  // previous state of pin

float temp_new      = LOW;  // current state of pin
float temp_old      = LOW;  // previous state of pin
//------------------------------------------------------
String fire_event = "ENTER_EVENT_CODE";
String gas_event  = "ENTER_EVENT_CODE";
String temp_event = "ENTER_EVENT_CODE";
String pir_event  = "ENTER_EVENT_CODE";
//String fire_event = "FIRE";
//String gas_event  = "GAS";
//String temp_event = "TEMPERATURE";
//String pir_event  = "MOTION";
//------------------------------------------------------
boolean buzzer_state = false;
unsigned long buzzer_timer = 0;
//------------------------------------------------------
int tank_height = 50; //in centimeter
int mq2_limit   = 55;
int temp_limit  = 50;
//------------------------------------------------------
#include <DHT.h>
#define DHTTYPE DHT22
DHT dht(DHT_PIN, DHTTYPE);
//------------------------------------------------------
//------------------------------------------------------
//--------------------------------------------------------------------------
// This function is called every time the device is connected 
//to the Blynk.Cloud Request the latest state from the server
BLYNK_CONNECTED() {
 Blynk.syncAll();
}
//--------------------------------------------------------------------------
/*// This function is called every time the Virtual Pin state change
//i.e when web push switch from Blynk App or Web Dashboard
BLYNK_WRITE(VPIN_BUTTON_1) {
  STATE_RELAY_1 = param.asInt();
  digitalWrite(RELAY_1, STATE_RELAY_1);
  Serial.print("Relay1 State = "); Serial.println(STATE_RELAY_1);
}
//--------------------------------------------------------------------------
BLYNK_WRITE(VPIN_BUTTON_2) {
  STATE_RELAY_2 = param.asInt();
  digitalWrite(RELAY_2, STATE_RELAY_2);
  Serial.print("Relay2 State = "); Serial.println(STATE_RELAY_2);
}
//--------------------------------------------------------------------------
BLYNK_WRITE(VPIN_BUTTON_3) {
  STATE_RELAY_3 = param.asInt();
  digitalWrite(RELAY_3, STATE_RELAY_3);
  Serial.print("Relay3 State = "); Serial.println(STATE_RELAY_3);
}
//--------------------------------------------------------------------------
BLYNK_WRITE(VPIN_BUTTON_4) {
  STATE_RELAY_4 = param.asInt();
  digitalWrite(RELAY_4, STATE_RELAY_4);
  Serial.print("Relay4 State = "); Serial.println(STATE_RELAY_4);
}
//--------------------------------------------------------------------------
BLYNK_WRITE(VPIN_PIR) {
  pirbutton = param.asInt();
  Serial.print("PIR Button = "); Serial.println(pirbutton);
}
//--------------------------------------------------------------------------
*/
/****************************************************************************************************
 * setup() Function
*****************************************************************************************************/
void setup(){
  // Debug console
  Serial.begin(115200);
  //--------------------------------------------------------------------
  /*pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);
  pinMode(BUTTON_3, INPUT_PULLUP);
  pinMode(BUTTON_4, INPUT_PULLUP);
  //--------------------------------------------------------------------
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_3, OUTPUT);
  pinMode(RELAY_4, OUTPUT);
  //--------------------------------------------------------------------
  //During Starting all Relays should TURN OFF
  digitalWrite(RELAY_1, HIGH);
  digitalWrite(RELAY_2, HIGH);
  digitalWrite(RELAY_3, HIGH);
  digitalWrite(RELAY_4, HIGH);*/
  //--------------------------------------------------------------------
  //pinMode(FLAME, INPUT);
  pinMode(PIR, INPUT);
  //pinMode(ECHO, INPUT);
  //pinMode(TRIG, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  //--------------------------------------------------------------------
  dht.begin();
  //--------------------------------------------------------------------
  //IrReceiver.begin(IR_RECEIVE_PIN); // Start the IR receiver
  //--------------------------------------------------------------------
  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
  //--------------------------------------------------------------------
  //Blynk.virtualWrite(VPIN_BUTTON_1, STATE_RELAY_1);
  //Blynk.virtualWrite(VPIN_BUTTON_2, STATE_RELAY_2);
  //Blynk.virtualWrite(VPIN_BUTTON_3, STATE_RELAY_3);
  //Blynk.virtualWrite(VPIN_BUTTON_4, STATE_RELAY_4);
  //--------------------------------------------------------------------
  timer.setInterval(100L, MQ2GasSensor);
  timer.setInterval(100L, DHT22Sensor);
  timer.setInterval(100L, PirSensor);
  //timer.setInterval(100L, UltrasonicSensor);
  //timer.setInterval(100L, FlameSensor);
}

/****************************************************************************************************
 * loop() Function
*****************************************************************************************************/
void loop() {
  Blynk.run();
  timer.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
  
  //listen_push_buttons();
  //listen_ir();
  
  if (buzzer_state == true) {
    if (millis() - buzzer_timer > 5000) {
      digitalWrite(BUZZER, LOW);
      buzzer_state = false;
      buzzer_timer = 0;
    }
  }
  
}

/****************************************************************************************************
 * DHT22Sensor Function
*****************************************************************************************************/
void DHT22Sensor() {
  //-----------------------------------------------------------------------
  //Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  //-----------------------------------------------------------------------
  //Read temperature as Celsius (the default)
  temp_old = temp_new;
  temp_new = dht.readTemperature();
  //-----------------------------------------------------------------------
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(temp_new)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  //-----------------------------------------------------------------------
  //Serial.print("Temperature: "); Serial.println(temp_new);
  //Serial.print("Humidity: "); Serial.println(h);
  //Serial.println("------------------------------------------");
  //-----------------------------------------------------------------------
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(VPIN_TEMPERATURE, temp_new);
  Blynk.virtualWrite(VPIN_HUMIDITY, h);
  //-----------------------------------------------------------------------
  //Serial.print(String(temp_old)+"-");Serial.println(temp_new);
  if(temp_old < temp_limit && temp_new >= temp_limit) { //LOW to HIGH
    String text = "Temperature is very high";
    Serial.println(text);
    Blynk.logEvent(temp_event, text);
    digitalWrite(BUZZER, HIGH);
    buzzer_state = true;
    buzzer_timer = millis();
  }
}

/****************************************************************************************************
 * PirSensor Function
*****************************************************************************************************/
void PirSensor() {
  if (pirbutton == 1) {
    pir_old = pir_new; // store old state
    pir_new = digitalRead(PIR); //read new state
    //------------------------------------------------------------------------
    if(pir_old == LOW && pir_new == HIGH) {
      String text = "Motion is Detected.";
      Serial.println(text);
      Blynk.logEvent(pir_event, text);
      digitalWrite(BUZZER, HIGH);
      buzzer_state = true;
      buzzer_timer = millis();
    }
    //------------------------------------------------------------------------
    else if(pir_old == HIGH && pir_new == LOW) { //HIGH to LOW
      Serial.println("Motion stopped!");
      digitalWrite(BUZZER, LOW);
    }
    //------------------------------------------------------------------------
  }
}

/****************************************************************************************************
 * MQ2GasSensor Function
*****************************************************************************************************/
void MQ2GasSensor() {
  mq2_old = mq2_new; // store old state
  mq2_new = analogRead(MQ2); //read new state
  mq2_new = map(mq2_new, 0, 4095, 0, 100);
  //Serial.print("MQ2GasSensor Value: "); Serial.println(mq2_new);

  //--------------------------------------------------------------
  if(mq2_old < mq2_limit && mq2_new >= mq2_limit) { //LOW to HIGH
    String text = "Gas leakage is detected";
    Serial.println(text);
    Blynk.logEvent(gas_event, text);
    digitalWrite(BUZZER, HIGH);
    buzzer_state = true;
    buzzer_timer = millis();
  }
  //--------------------------------------------------------------
  //else if(mq2_old > mq2_limit && mq2_new <= mq2_limit) { //HIGH to LOW
    //Serial.println("Fire stopped!");
    //digitalWrite(BUZZER, LOW);
  //}
  //--------------------------------------------------------------
  Blynk.virtualWrite(VPIN_MQ2, mq2_new);
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
BLYNK_WRITE(V4) {
  bool RelayTwo = param.asInt();
  if (RelayTwo == 1) {
    digitalWrite(relay2, LOW);
  } else {
    digitalWrite(relay2, HIGH);
  }
}
