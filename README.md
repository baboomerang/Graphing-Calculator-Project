# Graphing-Calculator-Project
Graphing Calculator Project as a Replacement for The Ti-84
This is designed utilizing multiple libraries for keypad, lcd, graphics and button debouncing.
Ti-84's are expensive as heck and I figured it couldn't be too hard to replicate functionality.
I guess I was wrong.....

# Materials
3.5" TFT 320x480 + Touchscreen Breakout Board w/MicroSD Socket - HXD8357D x 1

ATMEL ATTINY84-20PU 8-BIT 20MHz MCU (2 PACK) x 1

SunFounder Mega 2560 R3 ATmega2560-16AU Board (compatible with Arduino) x 2

Sparkfun Pocket AVR Programmer x 1

3mm and 5mm Assorted Clear LED Light Emitting Diodes 5 Colors Pack of 300

PIXNOR 1/4W Resistor Kit 56 Values x 20pcs =1120pcs (1ohm-10Mohm) Metal Film Resistors Assortment

CO-RODE Tact Button Switch 6x6x5mm Pack of 100

SainSmart IIC/I2C/TWI Serial 2004 20x4 LCD Module Shield For Arduino UNO MEGA R3 x 1


# Project Author(s)
Ilan Rodriguez

Credits to Code Authors: 

Kris Kasprzak for Graphing Code

RobTillaart for Arduino Code
  https://github.com/RobTillaart
  
Jeremy Gillick for LED FADER CODE
  https://github.com/jgillick/arduino-LEDFader

# Code Explanation

First, parsing mathematical expressions in an efficient manner is quite difficult unless you have a pre-processing algorithm. (that works for all the operators possible in a scientific calculator)

http://www.oxfordmathcenter.com/drupal7/node/628
https://en.wikipedia.org/wiki/Shunting-yard_algorithm

