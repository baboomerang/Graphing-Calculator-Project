/* @file attiny84keypadserial
  || @version 1.0
  || @author Mark Stanley
  || @3rd party editor Ilan Rodriguez
  || @contact mstanley@technologist.com
  ||
  || @description
  || | The latest version, 3.0, of the keypad library supports up to 10
  || | active keys all being pressed at the same time. This sketch is an
  || | example of how you can get multiple key presses from a keypad or
  || | keyboard.
  || #
*/

#include <Keypad.h>
#include "SoftwareSerial.h" //softwareserial
#include <LEDFader.h>
#include <Curve.h>

#define STATUS_LED 7
#define TRANSMIT_LED 8


// ADD PINS HERE FOR DIVISION AND SUBTRACTION

const int Rx = 10; // this is physical pin 2
const int Tx = 9; // this is physical pin 3
//const int led = 7;

SoftwareSerial tinySerial(Rx, Tx);
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'C', '0', '='}
};
byte rowPins[ROWS] = {6, 5, 4, 3}; //connect to the row pinouts of the kpd
byte colPins[COLS] = {2, 1, 0}; //connect to the column pinouts of the kpd

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup() {
  tinySerial.begin(9600);
  pinMode(Rx, INPUT);
  pinMode(Tx, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  pinMode(TRANSMIT_LED, OUTPUT);
}

void loop() {
  char keypressed = kpd.getKey();
  if (keypressed) {
    tinySerial.write(keypressed);
    digitalWrite(TRANSMIT_LED, LOW);
  }
  digitalWrite(STATUS_LED, HIGH);   // turn the LED on (HIGH is the voltage level)
}  // End loop

