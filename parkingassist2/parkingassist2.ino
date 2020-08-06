#include <EEPROM.h>

// Initialize variables
unsigned long  currentTime = 0;
unsigned long  lastTime = 0; // Used for turning off leds when timer is up
int timer = 60000; // milliseconds until lights will turn off
int triggerPin = 7; // Ultrasonic Trigger Output
int echoPin = 8; // Ultrasonic Trigger Return Input
long duration; // Ultrasonic measurement
int redLight = 11; // Red light pin
int yellowLight = 10; // Yellow light pin
int greenLight = 9; // Green light pin
int minButtonPin = 6; // Pin input for push button to trigger minimum stop distance (to clear front of garage)
int maxButtonPin = 4; // Pin input for push button to trigger maximum stop distance (to clear garage door)
int currentDistance = 0; // Distance in inches left until collision
int lastDistance = 0; // Distance last measured
int minDistance[3] = {minButtonPin, 0, 0}; // Pin, Inches, EEPROM storage address
int maxDistance[3] = {maxButtonPin, 0, 1}; // Pin, Inches, EEPROM storage address

void setup() {
  //Open serial port
  Serial.begin(9600);
  // Assign inputs and outputs
  minDistance[1] = EEPROM.read(0);
  maxDistance[1] = EEPROM.read(1);
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(redLight, OUTPUT);
  pinMode(yellowLight, OUTPUT);
  pinMode(greenLight, OUTPUT);
  pinMode(minButtonPin, INPUT);
  pinMode(maxButtonPin, INPUT);

}


void loop() {
  currentTime = millis();
  currentDistance = measureDistance();
  checkButtonPress(minDistance);
  checkButtonPress(maxDistance);
  updateStoplight(); //Figure out the LED pin state, then write to the LED pins
  Serial.print("The distance is: ");
  Serial.println(currentDistance); 
  delay(1000);
}

int measureDistance() {
  // Echo code adapted from: https://randomnerdtutorials.com/complete-guide-for-ultrasonic-sensor-hc-sr04/
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
  // Check for out or range or garbage data and return last measurement not current measurment
  if (duration > 17760) {
    return lastDistance;
  }
  // Convert the time into a distance
  // Divide by 74 or multiply by 0.0135 to get current distance away in inches
  return (duration / 2) / 74;
}

int checkButtonPress(int arr[]) { // Read and assign pin
  int pin = arr[0]; // Read pin location from array
  int state = digitalRead(pin);
  if (state == 1) {
    delay(50);
    if (digitalRead(pin) == 1) { // If pin state is still high after small delay
      arr[1] = currentDistance;
      int address = arr[2];
      EEPROM.update(address, currentDistance);
      Serial.print(arr[1]);
      Serial.println(" inches updated");
      writeLeds(3000, HIGH, HIGH, HIGH); // Send 3 second all LEDs ON to confirm update
      writeLeds(0, LOW, LOW, LOW); // Turn off all LEDs
      return;
    }
  }
}

int writeLeds(int timer, int red, int yellow, int green) {
  digitalWrite(redLight, red);
  digitalWrite(yellowLight, yellow);
  digitalWrite(greenLight, green);
  delay(timer);
}

void updateStoplight() {
  // If current distance is within +- one inch of the previous measurement
  if (lastDistance - 1 >= currentDistance && currentDistance <= lastDistance + 1) {
    lastTime = currentTime;
    Serial.println("Distance changed, timer reset");
  }
  lastDistance = currentDistance;
  //Serial.println((currentTime - lastTime) / 1000);
  // Turn off lights if not registering a change
  if (currentTime - lastTime > timer) {
    writeLeds(0, LOW, LOW, LOW);
    return;
  }

  // Min and Max values incorrectly inputted
  int mins = minDistance[1];
  int maxs = maxDistance[1];
  if (mins > maxs) {
    writeLeds(1000, HIGH, HIGH, HIGH);
    writeLeds(0, LOW, LOW, LOW);
    Serial.println("ERROR- Max distance is smaller than Min distance");
    return;
  }

  // Beyond maximum distance - Green Light
  if (currentDistance >= maxs) {
    writeLeds(0, LOW, LOW, HIGH);
    return;
  }

  //In between maximum and minimum distance - Yellow Light
  if (currentDistance < maxs && currentDistance > mins) {
    writeLeds(0, LOW, HIGH, LOW);
    return;
  }

  // Passed minimum distance- Red Light
  if (currentDistance <= mins) {
    writeLeds(0, HIGH, LOW, LOW);
    return;
  }
  Serial.println("SOMETHING HAPPENED... updateStopLight() did not work");
}
