


/* Ultrasonic sensor pin connections:
    VCC: +5VDC
    Trigger: Trigger Input (pin 11)
    Echo: Echo Outout (pin 12)
    Ground: GND
*/

// Pin Input & Outputs and Global Variables
int triggerPin = 13; // Ultrasonic Trigger Output
int echoPin = 12; // Ultrasonic Trigger Return Input
long duration; //cm, inches;
long inches;
int redCounter = 0;
int yellowCounter = 0;
int greenCounter = 0;
int redLight = 11; // Red light pin
int yellowLight = 10; // Yellow light pin
int greenLight = 9; // Green light pin
int button = 8; // Pin input for push button to trigger minimum stop distance
int buttonState = HIGH;
int triggerStop = 24; // Recorded minimum stop distance (trigger yellow light in inches)
int currentDistance; // Distance in inches left until collision
int collisionDistance = 12; //Distance less than this triggers red light in inches


void setup() {
  //Open serial port
  Serial.begin(9600);
  // Assign inputs and outputs
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(redLight, OUTPUT);
  pinMode(yellowLight, OUTPUT);
  pinMode(greenLight, OUTPUT);
  pinMode(button, INPUT);
}

void loop() {
  // Echo code taken from: https://randomnerdtutorials.com/complete-guide-for-ultrasonic-sensor-hc-sr04/
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(5);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);

  // Convert the time into a distance
  //cm = (duration / 2) / 29.1;   // Divide by 29.1 or multiply by 0.0343
  inches = (duration / 2) / 74; // Divide by 74 or multiply by 0.0135
  //lastDistance = currentDistance; // Assign last distance value
  currentDistance = inches; // Assign new current distance

  // Record triggerStop distance (minimum distance from sensor)
  buttonState = digitalRead(button);
  if (buttonState == LOW) {
    digitalWrite(redLight, HIGH);
    digitalWrite(yellowLight, HIGH);
    digitalWrite(greenLight, HIGH);
    delay(3000);
    digitalWrite(redLight, LOW);
    digitalWrite(yellowLight, LOW);
    digitalWrite(greenLight, LOW);
    triggerStop = currentDistance; // Assign trigger stop new distance
  }
  // Stop light logic

  // If distance is over stop distance turn on green light
  if (currentDistance >= triggerStop) {
    redCounter = 0;
    yellowCounter = 0;
    greenCounter++;
    // If light is on for 100 cycles, turn off light
    if (greenCounter >= 100) {
      greenCounter = 100;
      digitalWrite(redLight, LOW);
      digitalWrite(yellowLight, LOW);
      digitalWrite(greenLight, LOW);
      return;
    }
    digitalWrite(redLight, LOW);
    digitalWrite(yellowLight, LOW);
    digitalWrite(greenLight, HIGH);
  }

  // If distance is less than triggerStop distance and greater than collisionDistance, turn on yellow light
  if (currentDistance < triggerStop && currentDistance > collisionDistance) {
    redCounter = 0;
    yellowCounter++;
    greenCounter = 0;
    // If light is on for 100 cycles, turn off light
    if (yellowCounter >= 100) {
      yellowCounter = 100;
      digitalWrite(redLight, LOW);
      digitalWrite(yellowLight, LOW);
      digitalWrite(greenLight, LOW);
      return;
    }
    digitalWrite(redLight, LOW);
    digitalWrite(yellowLight, HIGH);
    digitalWrite(greenLight, LOW);

  }
  // If distance is less than collisionDistance, turn on red light
  if (currentDistance <= collisionDistance) {
    redCounter++;
    yellowCounter = 0;
    greenCounter = 0;
    // If light is on for 100 cycles, turn off light
    if (redCounter >= 100) {
      redCounter = 100;
      digitalWrite(redLight, LOW);
      digitalWrite(yellowLight, LOW);
      digitalWrite(greenLight, LOW);
      return;
    }
    digitalWrite(redLight, HIGH);
    digitalWrite(yellowLight, LOW);
    digitalWrite(greenLight, LOW);
  }


  /*
  Serial.print("Distance in inches: ");
  Serial.println(currentDistance);
  Serial.print("Trigger Stop Distance: ");
  Serial.println(triggerStop);
  Serial.println(greenCounter);
  */
  delay(250);
}
