# Graphing-Calculator-Project
This project is to serve as an alternative calculator to the infamous Ti-84

## Installation

Provide code examples and explanations of how to get the project.

## Materials
3.5" TFT 320x480 + Touchscreen Breakout Board w/MicroSD Socket - HXD8357D x 1

ATMEL ATTINY84-20PU 8-BIT 20MHz MCU (2 PACK) x 1

SunFounder Mega 2560 R3 ATmega2560-16AU Board (compatible with Arduino) x 2

Sparkfun Pocket AVR Programmer x 1

3mm and 5mm Assorted Clear LED Light Emitting Diodes 5 Colors Pack of 300

PIXNOR 1/4W Resistor Kit 56 Values x 20pcs =1120pcs (1ohm-10Mohm) Metal Film Resistors Assortment

CO-RODE Tact Button Switch 6x6x5mm Pack of 100

SainSmart IIC/I2C/TWI Serial 2004 20x4 LCD Module Shield For Arduino UNO MEGA R3 x 1


## Project Author(s)

Ilan Rodriguez

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

`char infixRAWnumberStack[300];`

`char infixstring[300]; // infix string buffer from serial input`

`byte infix_stack_reference[200]; `

`byte postfix_stack_reference[200]; `

`byte postfix_opstack[50]; `

`BigNumber numberStack_FINAL[50];`



http://www.oxfordmathcenter.com/drupal7/node/628

https://en.wikipedia.org/wiki/Shunting-yard_algorithm

First, infixstring[300] is initialized as a global array. As we recieve characters from serial, they get appended on in a L-R fashion.
The first value in the infix string is a Left Parenthesis. '('

Lets say we add in a 

## License

A short snippet describing the license (MIT, Apache, etc.)
