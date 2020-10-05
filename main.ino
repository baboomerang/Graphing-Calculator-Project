//#include <MemoryFree.h>
#include <BigNumber.h>
#include <EEPROM.h>

#include <avr/wdt.h>
#include "libs/stack.h"
#include "libs/list.h"

#define INPUT_SIZE 50
char input[INPUT_SIZE];
uint16_t index;

//mfw no assert() or exit()
void raise(byte value) {
  if (value == 0) Serial.println(F("SUCCESS"));
  if (value == 1) Serial.println(F("GENERAL SYNTAX ERROR"));
  if (value == 2) Serial.println(F("SYNTAX ERROR: NOT ENOUGH OPERANDS"));
  if (value == 3) Serial.println(F("SYNTAX ERROR: NOT ENOUGH OPERATORS"));
  if (value == 4) Serial.println(F("MEM OVERFLOWED: TOO LARGE NUMBER"));
  if (value == 5) Serial.println(F("INPUT: MAXIMUM SIZE REACHED"));
}

void setup() {
  wdt_disable();  //disable all interrupts and wdt; prevents reset loop
  wdt_setup();    //sets up watchdog and re-enables all interrupts
  
  BigNumber::begin(12);
  Serial.begin(19200);

  if (EEPROM.read(2) == 99) {
    EEPROM.write(2, 0); //clear memory error flag
    raise(4);   //notify user of memory error from last boot (when the ISR kicked in)
  }

  memset(input, 0, INPUT_SIZE);
  index = 0;

  Serial.println(F("READY"));
}

void loop() {
  wdt_reset();
  if (Serial.available() > 0) { //whenever the rx line on the avr goes high
    char token = Serial.read();
    switch (token) {
      case '(' ... '+': //from char 40 to 43; we skip the commma ',' (44)
      case '-' ... '9': //from char 45 to 57
      case '^':
        if (index > INPUT_SIZE) {
          raise(5);   //notify user the input is at max length
          break;
        }
        input[index++] = token;
        break;
      case 'c':
        memset(input, 0, INPUT_SIZE);
        index = 0;
        break;
      case 'p':
        Serial.print(F("INPUT: "));
        Serial.println(input); //debug
        break;
      case '=':
        calculate(input);
        break;
      case 'g':
        //graph(queue);
        break;
    }
  }
}

void printBignum(BigNumber &n) {
  char* s = n.toString();
  Serial.println(s);
  free(s);
}
