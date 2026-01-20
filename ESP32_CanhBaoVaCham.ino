#include <Wire.h> 
#include "SSD1306.h"

//Define variables
unsigned long startTime = 0;
unsigned long endTime = 0;
long distance = 0;
bool vehicleDetected = false;
bool IRDetected = false;
bool resetSensor1 = true;
bool resetSensor2 = true;
int velocity = 0;
int maxDistance = 40; 

SSD1306  display(0x3c, 21, 22);

//Define sensor pinout:
const int sensorPin1 = 16;
const int echoPin1 = 17;
const int sensorPin2 = 18;
const int echoPin2 = 19;

const int relayPin = 23;
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

void Show() {
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 5, String(velocity) + " Km/h");
    if (IRDetected)
      display.drawString(0, 20, "Lối 2: Có   ");
    else
      display.drawString(0, 20, "Lối 2: Không");
  if (velocity > 30) display.drawString(0, 40, "Chú ý");
    display.display();
}


void loop() {
  IRDetected = (digitalRead(sensorIR) == HIGH);
  if (IRDetected) 
    Serial.println("IR Detected");
  else
    Serial.println("No IR");
  
  Show();

  if (!vehicleDetected) {                                      
    readSensor(sensorPin1, echoPin1);                          
    
    if ((distance > 0 ) && (distance < maxDistance)) { 
      Serial.print("Sensor 1:");
      Serial.println(distance);                     
      startTime = millis();                                    // Start the timer for the passing car. 
      vehicleDetected = true;                                      // Change the boolean for car detection to true. 
      resetSensor1 = false;                                    // The sensor is now "used" and needs to be reset by a 0 measurement indicating that the current car has left the field. 
    }
  }

  if (vehicleDetected) {                          // If car is deteced on sensor 1, detect car on sensor 2.
    readSensor(sensorPin2, echoPin2);                          // Call the readSensorfunction and read sensor 1 
    
    if ((distance > 0)&&(distance < maxDistance)) {    
      Serial.print("Sensor 2:");
      Serial.println(distance);                  // Check if measurement fits with passing car (min, maks distance and if sensor is reset)
      endTime = millis();                                      // Start the timer for the passing car.
      velocity = round(3.6*130/(endTime-startTime));
      Serial.print("Velocity: ");
      Serial.println(velocity);
      Show();
      if (velocity > 30) {
        digitalWrite(relayPin, HIGH);
        Serial.println("On");
        Show();
        delay(2000);
      }
      vehicleDetected = false;                                     // Car has now clocked in at 1 & second sensor and we can reset the detection boolean. 
      resetSensor2 = false;                                    // 2 sensor is now "used" and needs to be reset by a 0 measurement indicating that the current car has left the field. 
    }
    
    if (millis() - startTime > 600) {                         // If sensor 1 detects a car that somehow never triggers the second sensor within 2 seconds - restart. 
      vehicleDetected = false;
      digitalWrite(relayPin, LOW);
      Serial.println("Too slow");
      display.clear();
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
