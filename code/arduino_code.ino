// ARDUINO FUNCTIONS AS A SLAVE DEVICE CONTROLLED VIA UART BY THE MSP432 (MASTER).
//
// Arduino executes commands received from the MSP432 to manage the following functions:
// - TURN ON LIGHTS      -> Command: 'O'
// - TURN OFF LIGHTS     -> Command: 'F'
// - OPEN THE WINDOWS    -> Command: 'D'
// - CLOSE THE WINDOWS   -> Command: 'H'
// - OPEN THE DOOR       -> Command: 'I' (followed by 'O')
// - CLOSE THE DOOR      -> Command: 'U'
//
// System includes an EARTHQUAKE DETECTION MECHANISM:
// - When the MSP432 detects an earthquake based on accelerometer data,
//   it sends the character 'T' to the Arduino.
// - Upon receiving this signal, the Arduino activates the red alert lights (e.g., LEDs)
//   to indicate a seismic emergency.
//
// Master-slave communication ensures synchronized control over home automation
// and safety mechanisms in both automatic (AUTO) and manual (EDIT) modes.

#include <SoftwareSerial.h>
#include <Servo.h>

// Serial communication with MSP432
SoftwareSerial mspSerial(10, 11); // RX, TX

// Servo motors for windows (1) and door (1)
Servo windowServo;
Servo doorServo;

// Pin configuration
const int windowPin = 9;     // Servo pin for window
const int doorPin = 6;       // Servo pin for door
const int whiteLightPin = 7; // Pin for white lights (3 LEDs)
const int redLightPin = 5;   // Pin for red lights (2 LEDs)

// Earthquake system state
bool earthquakeActive = false;
bool earthquakeOngoing = false;
unsigned long earthquakeStartTime = 0;
unsigned long blinkTimer = 0;
bool redLightState = false;

void setup() {
  mspSerial.begin(9600);
  Serial.begin(9600);

  // Attach servos
  windowServo.attach(windowPin);
  doorServo.attach(doorPin);

  // Set servos to neutral position
  windowServo.write(90);
  doorServo.write(90);

  // Configure light pins
  pinMode(whiteLightPin, OUTPUT);
  pinMode(redLightPin, OUTPUT);

  digitalWrite(whiteLightPin, LOW);
  digitalWrite(redLightPin, LOW);

  Serial.println("Arduino ready.");
}

void loop() {
  // Check for incoming commands from MSP432
  if (mspSerial.available()) {
    char command = mspSerial.read();
    Serial.print("Received command: ");
    Serial.println(command);

    switch (command) {
      // CLOSE WINDOWS
      case 'H':
        windowServo.write(0);
        mspSerial.write('C');
        Serial.println("Window CLOSED");
        break;

      // OPEN WINDOWS
      case 'D':
        windowServo.write(180);
        mspSerial.write('A');
        Serial.println("Window OPENED");
        break;

      // NEUTRAL WINDOW POSITION
      case 'N':
        windowServo.write(90);
        mspSerial.write('R');
        Serial.println("Window set to NEUTRAL");
        break;

      // OPEN DOOR
      case 'I':
        doorServo.write(180);
        Serial.println("DOOR OPENED");
        break;

      // CLOSE DOOR
      case 'U':
        doorServo.write(0);
        Serial.println("DOOR CLOSED");
        break;

      // TURN ON WHITE LIGHTS
      case 'O':
        digitalWrite(whiteLightPin, HIGH);
        Serial.println("WHITE LIGHT ON");
        break;

      // TURN OFF WHITE LIGHTS
      case 'F':
        digitalWrite(whiteLightPin, LOW);
        Serial.println("WHITE LIGHT OFF");
        break;

      // EARTHQUAKE ALERT: RED LIGHT BLINKS
      case 'T':
        earthquakeOngoing = true;
        earthquakeStartTime = millis();
        blinkTimer = millis();
        Serial.println("EARTHQUAKE: Red LED blinking");
        break;
    }
  }

  // Earthquake alert blinking logic
  if (earthquakeOngoing) {
    if (millis() - blinkTimer >= 500) {
      blinkTimer = millis();
      redLightState = !redLightState;
      digitalWrite(redLightPin, redLightState ? HIGH : LOW);
    }

    // End earthquake after 5 seconds
    if (millis() - earthquakeStartTime >= 5000) {
      digitalWrite(redLightPin, LOW);
      redLightState = false;
      earthquakeOngoing = false;
      Serial.println("EARTHQUAKE: Red LED OFF");
    }
  }
}

