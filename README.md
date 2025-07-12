# SmartHome
Repository for the Embedded Software for the IoT project at the University of Trento.

# Members
- [Ismaele Landini](https://github.com/Ismaele-landini)
- [Sophie Motter](https://github.com/sophiee03)
- [Tommaso Michelotti](https://github.com/Tommo-Tom)
- [Norris Kervatin](https://github.com/kerva17)

# Description and Functionality
Our project consist in creating a Smart Home that can provide many functionality exploiting the sensors of microcontrollers, including turning on the lights if someone enters the house and turn them off if someone exits, continuously check the temperature inside the abitation to open/close the windows in case of high/low temperature, checking constantly updated environment parameters on the display and alert people inside the house if an earthquake occurs by opening the door, switching on an alarm light and buzzer and by signaling the situation on the display. 
In addition we have introduced an edit mode that allows for testing: by clicking the joystick pushbutton it is possible to enter the edit mode, while, switching left the joystick allow to exit edit mode. The edit mode consists of these options that can be selected by the user:
- turn on lights
- turn off lights
- open windows
- close windows
- enter (open door and turn on lights)
- exit (open door and turn off lights)

***NB:*** To have a schematic view of the functionalities check out the [Flowchart Diagram](https://github.com/sophiee03/SmartHome/blob/main/files/Flowchart.pdf)

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
```plaintext
├── README.md
├── files                       # contains useful files
│   ├── Smart_Home.pdf          # presentation for the exam
│   ├── Smart_Home_diagram.png  # diagram of the connections
│   ├── Flowchart.pdf           # diagram to see the flow of the project
|   └── SmartHome.drawio.png    # image showing the project
└── code
    ├── msp_code.ino          # code for the MSP432 with Energia.nu
    └── arduino_code.ino      # code for aurduino with Arduino Uno 
```
# Steps to Run (Hardware side)
The connections that we have made to implement this project are the following:
![Schema collegamenti](files/Smart_Home_diagram.png)

# Steps to Run (Software side)
In order to test this project the first thing to do is having these developing environment installed and setup: [Arduino Uno](https://www.arduino.cc/en/software/) and [Energia.nu](https://energia.nu/download/).
Then the files in the [code](https://github.com/sophiee03/SmartHome/tree/main/code) folder can be dowloaded and opened in their editor.
Finally after setting up the hardware part, it is only necessary to compile and run on your PC.

# [Presentation]()

# [YouTube Video]()


