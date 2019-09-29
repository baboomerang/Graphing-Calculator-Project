/* @file keypad
  || @version 1.1
  || @author: Ilan Rodriguez
  || @credits to Mark Stanley for providing the library
  || #
*/

// Keypad Input - Attiny84
/* Designed to send input to the Calculator.
   This holds all the functions already embedded into the Calculator Program
*/


#include <Keypad.h>
#define tx 8

const byte ROWS = 5; //five rows
const byte COLUMNS = 4; //five columns


const char keys[ROWS][COLUMNS] = {
  {'(', ')', '^', '/'},
  {'1', '2', '3', '*'},
  {'4', '5', '6', '-'},
  {'7', '8', '9', '+'},
  {'.', '0', 'c', '='}
};

// ' ' are placeholders for extra buttons in the future

const byte rowPins[ROWS] = {8, 7, 6, 5, 4}; //connect to the row pinouts of the kpd
const byte colPins[COLUMNS] = {3, 2, 1, 0}; //connect to the column pinouts of the kpd
/*  keypad visualized
    pin 0    1    2    3    // each button is a vector that can be easily expanded but this is how the keypad should be wired to the attiny
       [ ]  [ ]  [ ]  [ ] 9
       [ ]  [ ]  [ ]  [ ] 8
       [ ]  [ ]  [ ]  [ ] 7
       [ ]  [ ]  [ ]  [ ] 6
       [ ]  [ ]  [ ]  [ ] 5
*/
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLUMNS );

void setup() {
  //  pinMode(tx, OUTPUT);
}

void loop() {
  if (kpd.getKey()) analogWrite(tx, kpd.getKey());  //writes out of assigned pin as analog value between 0-255 (uint_8t)
}


