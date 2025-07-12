# SmartHome
Repository for the Embedded Software for the IoT project at the University of Trento.

# Members
- [Ismaele Landini](https://github.com/Ismaele-landini)
- [Sophie Motter](https://github.com/sophiee03)
- [Tommaso Michelotti](https://github.com/Tommo-Tom)
- [Norris Kervatin](https://github.com/kerva17)

# Description and Functionality
Our project consist in creating a Smart Home that can provide the following functionality:
- by pressing the button1 of the MSP432 the door will be opened to enter the house, and with another press it will be reopened to leave the house
- when someone enter the house with the button1 press the lights will be turned on
- when someone leaves the house with the re-press of button1 the lights will be turned off
- when the temperature sensor detects a temperature higher than 27°, the windows will be opened, otherwise they will be closed
- when the accelerometer monitor the presence of earthquakes (oscillation from the normal position), the house will signal it with door opening and alarm light turned on
- the display of the MSP432 will show the parameters constantly updated (temperature, light ON/OFF, manual mode)

In addition we have introduced a manual mode that allows for testing (by pushing for 3 seconds the button2 of the MSP432):
- by pressing button1 of the MSP432 the lights will be turned on
- by pressing button2 of the MSP432 the windows will be opened
- by pressing button2 for 3s the system will return in automatic mode

# Hardware Components
The equipment we used is:
- MSP432 Launchpad
- MSP432 Boosterpack Sensors
- Arduino Uno
- Microservi SG90
- Led and diode
- Breadboard
- Resistors (220 Ohm)
- Converter

# Software Environment
- Arduino Uno
- Energia.nu

# Project Layout
├── SMART_HOME.md

├── files                     # contains useful files

│   ├── Smart_Home.pdf        # presentation for the exam

│   └── SH.png                # image showing the project

└── code

    ├── msp_code.ino          # code for the MSP432 with Energia.nu
    
    └── arduino_code.ino      # code for aurduino with Arduino Uno 
    

# Steps to Run (Hardware side)
The connections that we have made to implement this project are the following (consider C as converter, A as Arduino Uno and M as MSP432):


# Steps to Run (Software side)
In order to test this project the first thing to do is having these developing environment installed and setup: [Arduino Uno](https://www.arduino.cc/en/software/) and [Energia.nu](https://energia.nu/download/).
Then the file in the [code](https://github.com/sophiee03/SmartHome/code) folder can be dowloaded and opened in their editor.
Finally after setting up the hardware part, it is only necessary to compile and run on your PC.

# User Guide

# [Presentation]()

# Contribution

