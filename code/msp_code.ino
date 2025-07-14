// MSP432 FUNCTIONS AS A MASTER DEVICE CONTROLLING ARDUINO (SLAVE) VIA UART
//
// The MSP432 acts as the master controller of a smart home system, communicating with an Arduino via UART.
// It manages all operations executed by the Arduino, leveraging both sensor data and user interactions through a menu.
// The system features two main operating modes, accessible via a display-based menu:
//
// - AUTO MODE:
//   Utilizes the onboard sensors of the MSP432 to automatically control smart home functions.
//   - Automatically opens or closes windows based on temperature thresholds.
//   - Monitors room brightness using a light sensor.
//   - Door operations are triggered intelligently via physical buttons (entry/exit logic).
//
// - EDIT MODE:
//   Allows manual control of the smart home system via menu options on the MSP432 display.
//   The following operations are available:
//     - TURN ON LIGHTS
//     - TURN OFF LIGHTS
//     - OPEN WINDOWS
//     - CLOSE WINDOWS
//     - OPEN DOOR
//     - CLOSE DOOR
//
// EARTHQUAKE DETECTION:
// - The MSP432 continuously monitors accelerometer data to detect earthquakes.
// - Upon detection, it sends a 'T' command to the Arduino via UART.
// - The Arduino responds by activating red alert lights (e.g., blinking LEDs) to signal the emergency.
//
// This master-slave architecture ensures coordinated and safe control of home automation,
// supporting both autonomous and manual operation modes.

#include <Wire.h>
#include "Screen_HX8353E.h"    // Library for TFT display
#include "Adafruit_TMP006.h"   // Temperature sensor library
#include "OPT3001.h"           // Light sensor library

// Display and sensors
Screen_HX8353E myScreen;      // Display
Adafruit_TMP006 tmp006;       // Temperature sensor
opt3001 lightSensor;          // Lights sensor

// Joystick and buttons
const int joystickX = 2, joystickY = 26, joystickSel = 5;
const int buttonOne = 33;   // Button for entering
const int buttonTwo = 32;   // Button for exiting

// Buzzer and red LED for earthquake alert
const int buzzerPin = 40;
const int redLED = 39;

// Accelerometer pins and calibration
const int xpin = 23, ypin = 24, zpin = 25;
int calibX = 520, calibY = 517, calibZ = 720;  // Default calibration

// Sensor readings
float temperature = 0.0;
unsigned long lightRaw = 0;

// Menu and state control
enum State { MENU, AUTO, EDIT };
State currentState = MENU;   // Start at menu

int menuSelection = 0;       // Which item in main menu
int editSelection = 0;       // Which function in edit mode
const int numFunctions = 6;  // Number of EDIT functions

// Door and room tracking
bool inside = false;         // Current position 
bool doorMoving = false;     // Is door in motion?
unsigned long doorTimer = 0; // Timer for door movement

// Windows tracking
bool windowOpen = false;                  // Track window state
unsigned long lastWindowCheck = 0;        // Last time window auto logic ran


// Earthquake status
bool earthquake = false;
unsigned long earthquakeEndTimer = 0;

// Timers and flags
bool displayNeedsUpdate = true;
unsigned long lastJoyTime = 0;       // Last time joystick moved
unsigned long lastPressTime = 0;     // Last time button pressed
unsigned long lastAutoUpdate = 0;    // Last time sensors updated

// List of available functions in EDIT mode
const char* functions[] = {
  "Turn lights ON", "Turn lights OFF",
  "Open window", "Close window",
  "Enter", "Exit"
};

//
//  SETUP
//

void setup() {
  Serial.begin(9600);       // Debug serial (inside-system)
  Serial1.begin(9600);      // Communication with Arduino

  // Setup input and output pins
  pinMode(buttonOne, INPUT_PULLUP);
  pinMode(buttonTwo, INPUT_PULLUP);
  pinMode(joystickSel, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  pinMode(redLED, OUTPUT);
  digitalWrite(redLED, LOW);

  // Start sensors
  tmp006.begin(TMP006_CFG_8SAMPLE);
  lightSensor.begin();

  // Calibrate accelerometer, compute average position on 100 samples
  int sumX = 0, sumY = 0, sumZ = 0;
  for (int i = 0; i < 100; i++) {
    sumX += analogRead(xpin);
    sumY += analogRead(ypin);
    sumZ += analogRead(zpin);
    delay(5);
  }
  //Calibrate value
  calibX = sumX / 100;
  calibY = sumY / 100;
  calibZ = sumZ / 100;

  // Initialize screen
  myScreen.begin();
  myScreen.setOrientation(0);     // Horizontal orientation
  myScreen.setFontSolid(true);    // Background not transparent
  myScreen.setFontSize(2);
  myScreen.clear(blackColour);    // clear dispaly
  updateDisplay();                // Show initial display
}


//
//  LOOP
//

void loop() {
  // Read joystick and button
  int xJ = analogRead(joystickX), yJ = analogRead(joystickY);   //joystick coordinates
  bool sel = digitalRead(joystickSel) == LOW;                   //select button
  unsigned long now = millis();  // Get current time

  // Determine joystick direction
  bool up = (yJ > 1000 && xJ > 430 && xJ < 630);
  bool down = (yJ < 30 && xJ > 430 && xJ < 630);

  // Menu control logic with debounce
  if (now - lastJoyTime > 200) {
    if (currentState == MENU) {
      // Navigate main menu
      if (up && menuSelection > 0) { menuSelection--; displayNeedsUpdate = true; }
      else if (down && menuSelection < 1) { menuSelection++; displayNeedsUpdate = true; }
      else if (sel) {
        // Select AUTO or EDIT
        currentState = (menuSelection == 0) ? AUTO : EDIT;
        editSelection = 0;
        displayNeedsUpdate = true;
        lastPressTime = now;
      }
    //SELECT EDIT MODE
    } else if (currentState == EDIT) {
      // Navigate function menu
      if (up && editSelection > 0) { editSelection--; displayNeedsUpdate = true; }
      else if (down && editSelection < numFunctions - 1) { editSelection++; displayNeedsUpdate = true; }
      else if (sel) {
        executeFunction(editSelection);  // Run selected function
        displayNeedsUpdate = true;
      }
      // GO BACK (to menu): whether joystick moved left
      else if (xJ < 30 && yJ > 378 && yJ < 578 && now - lastPressTime > 1000) {
        currentState = MENU;
        menuSelection = 0;
        displayNeedsUpdate = true;
      }
    //SELECT AUTO MODE
    } else if (currentState == AUTO) {
      // Return from AUTO to MENU
      if (xJ < 30 && yJ > 378 && yJ < 578 && now - lastPressTime > 1000) {
        currentState = MENU;
        menuSelection = 0;
        displayNeedsUpdate = true;
      }
    }
    lastJoyTime = now;
  }

  // Automatic updates in AUTO mode every 4 seconds
  if (currentState == AUTO) {
    if (now - lastAutoUpdate > 4000) {
      updateSensors();           // Read sensors
      displayNeedsUpdate = true;
      lastAutoUpdate = now;
    }

    detectEarthquake();          // Check for earthquake

    // ENTRY (INSIDE): open door and turn on lights
    if (!doorMoving && digitalRead(buttonOne) == LOW && !inside) {
      doorMoving = true;
      doorTimer = now;
      Serial1.write('I'); delay(50); Serial1.write('O');
    }

    // EXIT (OUTSIDE): open door and turn off lights
    if (!doorMoving && digitalRead(buttonTwo) == LOW && inside) {
      doorMoving = true;
      doorTimer = now;
      Serial1.write('I'); delay(50); Serial1.write('F');
    }

    // After 5 seconds, close door
    if (doorMoving && now - doorTimer >= 5000) {
      Serial1.write('U');
      inside = !inside;
      doorMoving = false;
    }

    // Window control based on temperature 
    if (now - lastWindowCheck > 4000) {  // Every 4 seconds
      if (temperature > 26.0 && !windowOpen) {
        Serial1.write('D');               // Open window
        windowOpen = true;
      } else if (temperature < 22.0 && windowOpen) {
        Serial1.write('H');               // Close window
        windowOpen = false;
      }
      lastWindowCheck = now;
    }
  }

  // EDIT mode
  if (currentState == EDIT) {
    detectEarthquake();  // Earthquake active here too

    // Close door after exit (5s delay)
    if (doorMoving && now - doorTimer >= 5000) {
      Serial1.write('U');
      inside = false;
      doorMoving = false;
    }
  }

  // Redraw screen if required
  if (displayNeedsUpdate) {
    updateDisplay();
    displayNeedsUpdate = false;
  }
}

//  FUNCTIONS

void detectEarthquake() {
  // Read current accelerometer values
  int xVal = analogRead(xpin), yVal = analogRead(ypin), zVal = analogRead(zpin);

  // Check if motion exceeds threshold
  bool shake = abs(xVal - calibX) > 120 || abs(yVal - calibY) > 120 || abs(zVal - calibZ) > 120;

  //Activate earthquake alert
  if (shake && !earthquake) {
    earthquake = true;
    tone(buzzerPin, 1000);              // Play buzzer
    digitalWrite(redLED, HIGH);         // Turn on red LED (MSP432)
    Serial1.write('T');                 // Alert Arduino (turn on red LED)
    earthquakeEndTimer = millis();
    displayNeedsUpdate = true;
  }

  // Stop alert after 5s of no shake
  if (!shake && earthquake) {
    if (millis() - earthquakeEndTimer >= 5000) {
      earthquake = false;
      noTone(buzzerPin);                // deactivate buzzer
      digitalWrite(redLED, LOW);        // Turn off red LED (MSP432)
      displayNeedsUpdate = true;
    }
  }

  // Reset timer if still shaking
  if (shake) {
    earthquakeEndTimer = millis();
  }
}


void updateSensors() {
  // Get temperature and light level
  temperature = tmp006.readObjTempC();
  lightRaw = lightSensor.readResult();
}

void updateDisplay() {
  myScreen.clear(blackColour);  // Clear screen

  if (earthquake) {
    myScreen.setFontSize(2);
    myScreen.gText(10, 20, "!!! EARTHQUAKE !!!", redColour);
    return;
  }

  // Display depending on current state
  // MENU STATE
  if (currentState == MENU) {
    myScreen.gText(10, 30, menuSelection == 0 ? "> AUTO" : "  AUTO", whiteColour);
    myScreen.gText(10, 60, menuSelection == 1 ? "> EDIT" : "  EDIT", whiteColour);
  // AUTO STATE
  } else if (currentState == AUTO) {
    myScreen.setFontSize(2);
    myScreen.gText(5, 10, "AUTO MODE", greenColour);
    myScreen.setFontSize(1);
    myScreen.gText(5, 35, "Temp: " + String(temperature, 1) + " C", cyanColour);
    myScreen.gText(5, 50, "Light: " + String(lightRaw) + " lux", cyanColour);
    myScreen.gText(5, 65, "Status: " + String(inside ? "INSIDE" : "OUTSIDE"), yellowColour);
  // EDIT STATE
  } else if (currentState == EDIT) {
    myScreen.setFontSize(1);
    for (int i = 0; i < numFunctions; ++i) {
      myScreen.gText(5, 20 + i*15, String((i == editSelection) ? ">" : " ") + functions[i], cyanColour);
    }
    myScreen.gText(5, 110, "< Back", yellowColour);
  }
}

void executeFunction(int idx) {
  // Perform selected function in EDIT mode
  switch (idx) {
    case 0: Serial1.write('O'); break;  // Turn ON lights
    case 1: Serial1.write('F'); break;  // Turn OFF lights
    case 2: Serial1.write('D'); break;  // Open window
    case 3: Serial1.write('H'); break;  // Close window
    case 4:                             // Enter Smart House
      Serial1.write('I');               //  Open door
      delay(50);
      Serial1.write('O');
      inside = true;
      break;
    case 5:                             // Exit Smart House
      Serial1.write('I');
      delay(50); 
      Serial1.write('F');
      Serial1.write('U');               // Close door 
      inside = false;
      doorMoving = false;
      break;
  }
}

