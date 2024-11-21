void setup() 
{
  pinMode(16, OUTPUT);     // Initialize the 16 pin as an output
  pinMode(4, OUTPUT);     // Initialize the 16 pin as an output
}

// the loop function runs over and over again forever
void loop() 
{
  digitalWrite(16, HIGH);   // Turn the LED on by making the voltage LOW
  delay(6000);                      // Wait
  digitalWrite(16, LOW);  // Turn the LED off by making the voltage HIGH
  delay(2000);                      // Wait

  digitalWrite(4, LOW);   // Turn the LED on by making the voltage LOW
  delay(1000);                      // Wait
  digitalWrite(4, HIGH);  // Turn the LED off by making the voltage HIGH
  delay(3000);                      // Wait
}
