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
#include <Bounce2.h>

#define BUTTON_PIN_ADD 8
#define BUTTON_PIN_SUBTRACT 6


// ADD PINS HERE FOR DIVISION AND SUBTRACTION

const int Rx = 10; // this is physical pin 2
const int Tx = 9; // this is physical pin 3
const int led = 7;

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

int oldaddvalue = LOW;
int oldsubvalue = LOW;

Bounce debouncer = Bounce(); // debouncer object
Bounce debouncer2 = Bounce(); // debouncer object

void setup() {
  tinySerial.begin(9600);
  pinMode(Rx, INPUT);
  pinMode(Tx, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(BUTTON_PIN_ADD, INPUT_PULLUP);
  pinMode(BUTTON_PIN_SUBTRACT, INPUT_PULLUP);
  debouncer.attach(BUTTON_PIN_ADD);
  debouncer2.attach(BUTTON_PIN_SUBTRACT);
  debouncer.interval(10); // interval in ms
  debouncer2.interval(10);
}

void loop() {

  debouncer2.update();
  debouncer.update();
  add_debounce();
  sub_debounce();

  int value2 = debouncer2.read();
  int value = debouncer.read();

  char keypressed = kpd.getKey();
  if (keypressed) {
    tinySerial.write(keypressed);
  }

  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)             // wait for a second

}  // End loop

void add_debounce() {
  // Update the Bounce instance :
  debouncer.update();
  // Get the updated value :
  int value = debouncer.read();
  if (value != oldaddvalue) { // if the current read was the same as before, dont do any of this other code until it changes
    if ( value == LOW ) {
    } else {
      tinySerial.write(43);
    }
  } // end of the (IF value is != old value )
  oldaddvalue = value;
} // end ============================================================================

void sub_debounce() {
  // Update the Bounce instance :
  debouncer2.update();
  // Get the updated value :
  int value2 = debouncer2.read();
  if (value2 != oldsubvalue) { // if the current read was the same as before, dont do any of this other code until it changes
    if ( value2 == LOW ) {
    } else {
      tinySerial.write(45);
    }
  } // end of the (IF value is != old value )
  oldsubvalue = value2;
} // end ============================================================================
