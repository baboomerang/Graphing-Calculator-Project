# Graphing-Calculator-Project
This project is to serve as an alternative calculator to the infamous Texas Instruments Ti-84 Graphing Calculator. 
It runs multiple Atmel AVR chips to process inputs, perform calculations and then drive the actual display. There is a multiplexed keypad along with 8 debounced buttons for serial input. An Attiny84 sends keypad input along with an Atmega328p to the Arduino Mega 2560. The Arduino Mega uses that information to perform operations and graph functions similar to how a Ti-84 would do it. (Not quite, but similar). There are also status LED's implemented for visual debug and aesthetic.

### Project Author(s)

Ilan Rodriguez

### Motivation
I needed a calculator earlier in my senior high school year, so I decided to build one from scratch. However, it took significantly longer than expected. Thus, by the time I finished, I didn't actually need it. It's still cool to have nonetheless. Plus, making a calculator at this level really exercises my programming and planning skills.

  Originally, I built BCD Full-Adders in minecraft with redstone and mods. Once my computer couldn't run all demands of the game at that stage, I decided to build a computer through real-life hardware. However, I do not know enough Computer Engineering design to build a computer from pure logic chips. 

Building this calculator is indirectly the closest alternative I can do that brings a tangible product as soon as possible.

### Installation
1. Download and install Software-Serial, Bounce2, TinyCore, MiniCore, BigNumber, LEDFader, and MemoryFree Libraries listed below.
2. Get a Sparkfun TinyAVR Programmer or any other compatible AVR programmer.
3. Install the [*attiny84keypadserial.ino*](../master/attin84keypadserial/attin84keypadserial.ino) onto an Atmel Attiny 84.
4. Then install the [*bounce2buttonsatmega328p.ino*](../master/bounce2buttonsatmega328p/bounce2buttonsatmega328p.ino) onto an Atmel Atmega328p (Arduino UNO or barebones breadboard-able chip).
5. Use [*graphingfuncinfix.ino*](../master/graphingfuncinfix/graphingfuncinfix.ino) and install that to an arduino mega2560.

### Materials
*3.5" TFT 320x480 + Touchscreen Breakout Board w/MicroSD Socket - HXD8357D* x 1

*ATMEL ATTINY84-20PU 8-BIT 20MHz MCU (2 PACK)* x 1

*SunFounder Mega 2560 R3 ATmega2560-16AU Board (compatible with Arduino)* x 2

*Sparkfun Pocket AVR Programmer* x 1

*3mm and 5mm Assorted Clear LED Light Emitting Diodes 5 Colors Pack of 300*

*PIXNOR 1/4W Resistor Kit 56 Values x 20pcs =1120pcs (1ohm-10Mohm) Metal Film Resistors Assortment*

*CO-RODE Tact Button Switch 6x6x5mm Pack of 100*

*SainSmart IIC/I2C/TWI Serial 2004 20x4 LCD Module Shield For Arduino UNO MEGA R3* x 1


## Additional Credits to Library Authors:

###### Kris Kasprzak for Graphing Library

###### Thomas O. Fredericks for Bounce2 Library

  https://github.com/thomasfredericks/Bounce2

###### Sudar Muthu for MemoryFree Library

  https://github.com/sudar/MemoryFree
  
###### RobTillaart for Arduino Code

  https://github.com/RobTillaart
  
###### Jeremy Gillick for LED Fader Library

  https://github.com/jgillick/arduino-LEDFader
  
###### Nick Gammon for BigNumber Library

  https://github.com/nickgammon/BigNumber
  
###### MCUdude for providing board definitions on non-standard Arduino Chips

  https://github.com/MCUdude/MightyCore and https://github.com/MCUdude/MiniCore
  
###### SainSmart Manufacturer for providing the library in handling the 20x4 LCD

## Code Explanation

##### Global Variables
Given Equation:  3 + 4 * 2 / (1 - 5) ^ 2 ^ 3

``` cpp
char infixRAWnumberStack[300];  300 element 2-D Array that holds all the numbers 3 4 2 1 5 2 3 in order.

char infixstring[300];  Another 300 element 2-D Array that buffers the infix string. "3+4*2/(1-5)^2^3"

byte infix_stack_reference[200];  Holds an indexed version of the input infix string for easier processing

byte postfix_stack_reference[200]; Holds an indexed version of the output postfix string

byte postfix_opstack[50]; Holds the operators read from the infix stack

BigNumber numberStack_FINAL[50]; Holds the actual numbers in which operations are performed on.
```

First, infixstring[300] is initialized as a global array. As we recieve characters from serial, they get appended on in a L-R fashion.
The first value in the infix string is a Left Parenthesis. '(' and then the inputted expression.

`Recieved infix string: (3+4*2/(1-5)^2^3`

When you want to solve the expression, you send the character 'g' over serial to the mega.

Then the infix expression gets capped with a final closing parenthesis. ' )'

*this will be updated later..... there is too much code to be explained...*

## Images
![img_graph](https://cloud.githubusercontent.com/assets/15847684/24832676/d123fc68-1c82-11e7-9c14-bde2d07e4cc0.jpg)

## License
    baboomerang/Graphing-Calculator-Project
    Copyright (C) <2017>  <Ilan Rodriguez>, <et-al>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
