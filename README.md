# Graphing-Calculator-Project
This project is to serve as an alternative calculator to the infamous Texas Instruments Ti-84 Graphing Calculator. 
It uses a single Atmega2560 to perform calculations and then drive the actual display. There is a multiplexed keypad along with 8 debounced buttons for input. The Arduino Mega uses that information to perform operations and graph functions similar to how a Ti-84 would do it. (Not quite, but similar). There are also status LED's implemented for visual debug and aesthetic.

### Project Author(s)

Ilan Rodriguez

### Motivation
I needed a calculator earlier in my senior high school year, so I decided to build one from scratch. However, it took significantly longer than expected. Thus, by the time I finished, I didn't actually need it. It's still cool to have nonetheless. Plus, making a calculator at this level exercised my programming and planning skills.

  Originally, I built BCD Full-Adders in minecraft with redstone and mods. Once my computer couldn't run all demands of the game at that stage, so I decided to build a computer with real hardware. However, I do not know enough Computer Engineering design to build a computer from discrete logic. 

Building this calculator is a closer alternative in that it can be made tangible sooner.

### Installation
1. Compile, and upload to an Arduino Atmega2560.
2. Plug TFT Display shield on the mega.

### Materials
*3.5" IPS TFT LCD Display ILI9486/ILI9488 480x320 36 Pins for Arduino Mega2560 * x 1

*ATMEL ATTINY84-20PU 8-BIT 20MHz MCU (2 PACK)* x 1

*SunFounder Mega 2560 R3 ATmega2560-16AU Board (compatible with Arduino)* x 2

*Sparkfun Pocket AVR Programmer* x 1

*3mm and 5mm Assorted Clear LED Light Emitting Diodes 5 Colors Pack of 300*

*PIXNOR 1/4W Resistor Kit 56 Values x 20pcs =1120pcs (1ohm-10Mohm) Metal Film Resistors Assortment*

*CO-RODE Tact Button Switch 6x6x5mm Pack of 100*

*SainSmart IIC/I2C/TWI Serial 2004 20x4 LCD Module Shield For Arduino UNO MEGA R3* x 1

## Additional Credits to Library Authors:

###### Kris Kasprzak for Graphing Library

###### Bodmer for HX8357B, HX8357C, ILI9481 and ILI9486 Libraries
  https://github.com/Bodmer/TFT_HX8357

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
