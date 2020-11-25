#include <BigNumber.h>
#include <EEPROM.h>
#include <MemoryFree.h>
#include <avr/wdt.h>

#include "libs/stack.h"
#include "libs/list.h"
#include "libs/queue.h"

enum direction {LEFT, RIGHT};
int const ERROR_BIT = 7;
int const INPUT_SIZE = 50;
String input;

/*
 * Insert test expression into your arduino serial monitor: ((3*5325)^7/3412*16)-12+(1555/12-29421)^2
 * Result should be this (in postfix notation): 3 5325 * 7 ^ 3412 / 16 * 12 - 1555 12 / 29421 - 2 ^+
 * Numerical Result : 1245079343380762145465942456.417651751986453041552689  or 1.245079 * 10^27
 */

//mfw no assert() or exit() thank you arduino
void raise(byte value) {
  if (value == 0) Serial.println(F("SUCCESS"));
  if (value == 1) Serial.println(F("GENERAL SYNTAX ERROR"));
  if (value == 2) Serial.println(F("SYNTAX ERROR: NOT ENOUGH OPERANDS"));
  if (value == 3) Serial.println(F("SYNTAX ERROR: NOT ENOUGH OPERATORS"));
  if (value == 4) Serial.println(F("SYNTAX ERROR: PARENTH MISMATCH"));
  if (value == 5) Serial.println(F("MEM ERROR: TOO LARGE NUMBER"));
  if (value == 6) Serial.println(F("INPUT: MAXIMUM SIZE REACHED"));
}

void setup() {
  wdt_disable(); //disable all interrupts and wdt; prevents reset loop
  wdt_setup(); //sets up watchdog and re-enables all interrupts
  BigNumber::begin(12);
  Serial.begin(19200);

  if (EEPROM.read(ERROR_BIT) == 99) {
    EEPROM.write(ERROR_BIT, 0); //clear memory error flag
    raise(5); //notify user of memory error from last boot (when the ISR kicked in)
  }

  input.reserve(INPUT_SIZE);
  Serial.println(F("READY"));
}

void loop() {
  wdt_reset();
  if (Serial.available() > 0) { //whenever the rx line on the avr goes high
    char token = Serial.read();
    switch (token) {
      case '(' ... '+': //char 40 to 43
      case '-' ... '9': //char 45 to 57
      case '^':
        if (input.length() < INPUT_SIZE)
          input.concat(token);
        else
          raise(6); //max input length reached
        break;
      case 'c':
        input.remove(0);
        break;
      case '=':
        calculate(input);
        break;
      case 'p':
        Serial.print(F("INPUT: "));
        Serial.println(input);
        break;
    }
  }
}
