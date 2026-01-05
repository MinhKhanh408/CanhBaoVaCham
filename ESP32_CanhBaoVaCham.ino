#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`

//Define variables
unsigned long startTime = 0;
unsigned long endTime = 0;
long distance = 0;
bool vehicleDetected = false;
bool IRDetected = false;
bool resetSensor1 = true;
bool resetSensor2 = true;
int velocity = 0; 
SSD1306  display(0x3c, 21, 22);

//Define sensor pinout:
const int sensorPin1 = 16;
const int echoPin1 = 17;
const int sensorPin2 = 18;
const int echoPin2 = 19;

const int relayPin = 34;
const int sensorIR = 32;
void setup() {
  
  Serial.begin(115200);

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

  //Define pinModes:
  pinMode(sensorPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(sensorPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(sensorIR, INPUT);

  //make sure sensorpins is low
  digitalWrite(sensorPin1, LOW);
  digitalWrite(sensorPin2, LOW);
  digitalWrite(relayPin, LOW);
  
}

void ShowSpeed() {
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_24);
    display.drawString(0, 26, String(velocity) + " Km/h");
}

void loop() {
  IRDetected = (digitalRead(sensorIR) == HIGH);
  if (!vehicleDetected) {                                          // Start reading sensor 1 if there is no car detected              
    readSensor(sensorPin1, echoPin1);                          // Call the readSensorfunction and read sensor 1 
    //Serial.print("Sensor 1:");
    //Serial.println(distance);
    if (distance < 200 && resetSensor1) {                      // Check if measurement fits with passing car (min, maks distance and if sensor is reset)
      startTime = millis();                                    // Start the timer for the passing car. 
      vehicleDetected = true;                                      // Change the boolean for car detection to true. 
      resetSensor1 = false;                                    // The sensor is now "used" and needs to be reset by a 0 measurement indicating that the current car has left the field. 
    } else if ((distance > 200)||(distance == 0)) {                                // If the sensor returns 0 (pulseIn timout), the sensor should be reset
      resetSensor1 = true;
    }
  }
  
  if (vehicleDetected || !resetSensor2) {                          // If car is deteced on sensor 1, detect car on sensor 2.
    readSensor(sensorPin2, echoPin2);                          // Call the readSensorfunction and read sensor 1 
    //Serial.print("Sensor 2:");
    //Serial.println(distance);
    if (distance < 200 && resetSensor2) {                      // Check if measurement fits with passing car (min, maks distance and if sensor is reset)
      endTime = millis();                                      // Start the timer for the passing car.
      velocity = round(1600/(endTime-startTime));
      display.clear();
      ShowSpeed();
      display.display();
      if (velocity > 5) digitalWrite(relayPin, HIGH);
      vehicleDetected = false;                                     // Car has now clocked in at 1 & second sensor and we can reset the detection boolean. 
      resetSensor2 = false;                                    // 2 sensor is now "used" and needs to be reset by a 0 measurement indicating that the current car has left the field. 
    } else if ((distance > 200 )||(distance == 0)) {                               // This handles the reset.
      resetSensor2 = true;
    }
    
    if (vehicleDetected && IRDetected) digitalWrite(relayPin, HIGH);

    if (millis() - startTime > 5000) {                         // If sensor 1 detects a car that somehow never triggers the second sensor within 2 seconds - restart. 
      vehicleDetected = false;
      digitalWrite(relayPin, LOW);
    }     
  }
}

//Function that reads sensor 1
void readSensor(int trigger, int echo) {                        // Function
  digitalWrite(trigger, HIGH);                                  // Trigger jsn-sr04t #1
  delayMicroseconds(20);                                        // Trigger jsn-sr04t #2
  digitalWrite(trigger, LOW);                                   // Trigger jsn-sr04t #3
  long timeElapsed = pulseIn(echo, HIGH, 40000);                // Measure time for echo - timeout set according to expected distance to cars (speeds up measurements)
  distance = timeElapsed * 0.0343 / 2;                          // Calculate distance in centimeters
  delay(1);                                                     // Small delay to prevent faulty readings (will give intermittent 0 readings if no delay)
}