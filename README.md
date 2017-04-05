# Graphing-Calculator-Project
This project is to serve as an alternative calculator to the infamous Texas Instruments Ti-84 Graphing Calculator. It runs multiple Atmel AVR chips to process inputs, perform calculations and then drive the actual display. There is a multiplexed keypad along with 8 debounced buttons for serial input. An Attiny84 sends keypad input along with an Atmega328p to the Arduino Mega 2560. The Arduino Mega uses that information to perform operations and graph functions similar to how a Ti-84 would do it. (Not quite, but similar). There are status LED's implemented for visual debug and aesthetic.

### Project Author(s)
=====

Ilan Rodriguez

=====
## Motivation
I needed a calculator earlier in my senior high school year, so I decided to build one from scratch. However, it took significantly longer than expected thus by the time I finished I didn't actually need it. It's still cool to have nonetheless. Plus, making a calculator at this level really exercises my programming and planning skills.

## Installation

Use a Sparkfun TinyAVR Programmer or some other type of programmer and install the *attiny84keypadserial.ino* onto an Atmel Attiny 84 
Then install the *bounce2buttonsatmega328p.ino* onto an Atmel Atmega328p (Arduino UNO or barebones breadboard-able chip)

## Materials
3.5" TFT 320x480 + Touchscreen Breakout Board w/MicroSD Socket - HXD8357D x 1

ATMEL ATTINY84-20PU 8-BIT 20MHz MCU (2 PACK) x 1

SunFounder Mega 2560 R3 ATmega2560-16AU Board (compatible with Arduino) x 2

Sparkfun Pocket AVR Programmer x 1

3mm and 5mm Assorted Clear LED Light Emitting Diodes 5 Colors Pack of 300

PIXNOR 1/4W Resistor Kit 56 Values x 20pcs =1120pcs (1ohm-10Mohm) Metal Film Resistors Assortment

CO-RODE Tact Button Switch 6x6x5mm Pack of 100

SainSmart IIC/I2C/TWI Serial 2004 20x4 LCD Module Shield For Arduino UNO MEGA R3 x 1


# Additional Credits to Library Authors:

###### Kris Kasprzak for Graphing Code

###### RobTillaart for Arduino Code

  https://github.com/RobTillaart
  
###### Jeremy Gillick for LED Fader Code

  https://github.com/jgillick/arduino-LEDFader
 
###### MCUdude for providing board definitions on non-standard Arduino Chips

  https://github.com/MCUdude/MightyCore and https://github.com/MCUdude/MiniCore
  
###### SainSmart Manufacturer for providing the library in handling the 20x4 LCD

# Code Explanation

##### Global Variables
Given Equation:  3 + 4 * 2 / (1 - 5) ^ 2 ^ 3

`char infixRAWnumberStack[300];`  300 element 2-D Array that holds all the numbers 3 4 2 1 5 2 3 in order.

`char infixstring[300]; /`  Another 300 element 2-D Array that buffers the infix string. "3+4*2/(1-5)^2^3"

`byte infix_stack_reference[200]; `  Holds an indexed version of the input infix string for easier processing

`byte postfix_stack_reference[200];` Holds an indexed version of the output postfix string

`byte postfix_opstack[50]; ` Holds the operators read from the infix stack

`BigNumber numberStack_FINAL[50];` Holds the actual numbers in which operations are performed on.


First, infixstring[300] is initialized as a global array. As we recieve characters from serial, they get appended on in a L-R fashion.
The first value in the infix string is a Left Parenthesis. '('


## License

A short snippet describing the license (MIT, Apache, etc.)
