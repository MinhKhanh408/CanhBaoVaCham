/* Arduino example sketch to control a JSN-SR04T ultrasonic distance sensor with Arduino. No library needed. More info: https://www.makerguides.com */

// Define Trig and Echo pin:
#define trig1Pin 16
#define echo1Pin 17
#define trig2Pin 9
#define echo2Pin 10

// Define variables:
long duration;
int distance;


long lastMsg = 0;
String str;
int limit;

void setup() {
  // Define inputs and outputs
  pinMode(trig1Pin, OUTPUT);
  pinMode(echo1Pin, INPUT);
  
  // Begin Serial communication at a baudrate of 9600:
  Serial.begin(9600);

}

void loop() {
  long now = millis();
  if (now - lastMsg > 3000) {
   lastMsg = now;
  //int str = 30;
    if (Serial.available() > 0) {
        str = Serial.readStringUntil('\n');
        limit = str.toInt();
        Serial.println(Serial.readStringUntil('\n'));
  ////      lcd.print(Serial.readStringUntil('\n'));
        delay(100);
    }
    else{

    // Clear the trigPin by setting it LOW:
    digitalWrite(trig1Pin, LOW);
    
    delayMicroseconds(5);

  // Trigger the sensor by setting the trigPin high for 10 microseconds:
    digitalWrite(trig1Pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig1Pin, LOW);
    
    // Read the echoPin. pulseIn() returns the duration (length of the pulse) in microseconds:
    duration = pulseIn(echo1Pin, HIGH);
    
    // Calculate the distance:
    distance = duration*0.0343/2;
    // Print the distance on the Serial Monitor (Ctrl+Shift+M):
    //Serial.print("Distance = ");
    Serial.println(distance);
    //Serial.println(" cm");
    }
  }
}