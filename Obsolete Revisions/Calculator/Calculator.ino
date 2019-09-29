#include <MemoryFree.h>

/* @file Calculator
  || @version 3.0
  || @date 04/04/2018
  || @author: Ilan Rodriguez
  || @credits to nickgammon, Efstathios Chatzikyriakidis, Bob101, & Gabriel for libraries.
  || #
*/

// Calculator Core - Atmega2560
/* This is the core of the project. It does all of the magic.
   IT WORKS WITH OR WITHOUT AN LCD. Just upload & write an expression
   Check your serial monitor */

#include <Arduino.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>

#include <BigNumber.h>

#include <EEPROM.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#include <QueueList.h>
#include <StackList.h>   // don't use QueueArray or StackArray, arduino behaves unexpectedly with arrays of dynamic sizes. (glitches, unexpected crashes)
#include <Wire.h>

#define I2C_ADDR 0x27
#define BACKLIGHT_PIN 3
#define En_pin 2
#define Rw_pin 1
#define Rs_pin 0
#define D4_pin 4
#define D5_pin 5
#define D6_pin 6
#define D7_pin 7

byte x = 0;
byte y = 0;
LiquidCrystal_I2C lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin); // set the LCD address to 0x38
QueueList <char> serialQueue;
/* ======================================================================================
  Alright, so....

   Insert test expression into your arduino serial monitor: ((3*5325)^7/3412*16)-12+(1555/12-29421)^2

   Result should be this (in postfix notation): 3 5325*7^3412/16*12-155512/29421-2^+
  ^ (if postfixOutput was enabled)

   Numerical Result : 1245079343380762145465942456.417651751986453041552689  or 1.245079 * 10^27

  This program converts infix expressions to postfix and solves on the fly.
  (If you want it to show a postfix expression, do this)
  ======================================================================================== */
ISR( WDT_vect ) {
  EEPROM.write(2, 99);  //ISR gives very little time to do any action before a system reset, writing error code to eeprom (which is non-volatile)is useful.
}

void watchdogSetup(void) {
  cli(); // disable all interrupts
  wdt_reset(); // reset the WDT timer
  /*
     WDTCSR configuration:
     WDIE = 1: Interrupt Enable
     WDE = 1 :Reset Enable
     WDP3 = 0 :For 2000ms Time-out
     WDP2 = 1 :For 2000ms Time-out
     WDP1 = 1 :For 2000ms Time-out
     WDP0 = 1 :For 2000ms Time-out
  */
  // Enter Watchdog Configuration mode:
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  // Set Watchdog settings:
  WDTCSR = (1 << WDIE) | (1 << WDE) | (0 << WDP3) | (1 << WDP2) | (1 << WDP1) | (1 << WDP0);
  sei();
}

void setup() {
  wdt_disable(); // disable watchdog immediately to prevent any bugs or bootloops
  watchdogSetup();

  Serial.print("freeMemory()=");
  Serial.println(freeMemory());

  BigNumber::begin(10);
  Serial.begin(19200);
  Serial1.begin(9600);
  serialQueue.push('(');

  lcd.begin (20, 4, LCD_5x8DOTS);
  lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
  lcd.setBacklight(HIGH);
  lcdsmartClear(lcd, 1);

  if (EEPROM.read(2) == 99) {
    Serial.println(F("MEM OVERFLOW: TOO LARGE NUMBER")); // Serial0 tells the connected computer what happened
    lcd.print(F("OVERFLOW # TOO LARGE")); // Same as above, but prints to lcd
    EEPROM.write(2, 0);
  }

}

void loop() {
  wdt_reset();
  if (Serial.available() > 0 || Serial1.available() > 0) { // ironically, this is to make sure everything in "void loop" only runs once for every time you input a character
    Serial.print("freeMemory()=");
    Serial.println(freeMemory());

    byte incomingbyte;
    if (Serial.available() > 0 ) incomingbyte = Serial.read(); //recieve byte from computer serial
    if (Serial1.available() > 0 ) incomingbyte = Serial1.read(); //recieve byte from other chips/devices

    if (char(incomingbyte) == '=') {   // the equalsign detection comes before everything to prevent it from being printed/pushed to the stack and causing errors

      Serial.print("freeMemory()=");
      Serial.println(freeMemory());

      lcdsmartClear(lcd, 1);
      serialQueue.push(')'); // caps the queue
      errorCheck(calculate(serialQueue));
      serialQueue.push('('); // once process_infix returns, the stack is inevitably cleared with no starting character. This is the workaround.

    } else if (char(incomingbyte) == 'c') {

      printStack(serialQueue); //stack is cleared automatically when it's printed to the console
      serialQueue.push('(');
      lcdsmartClear(lcd, 1);

    } else if ( ( isDigit(char(incomingbyte)) || isOperator(char(incomingbyte)) ) ) {

      if (x != 20 && y != 4) {
        serialQueue.push(char(incomingbyte)); // anything that isn't a control character is pushed to the stack
        Serial.println("Input: " + String(char(incomingbyte)));
        lcdsmartPrint(lcd, char(incomingbyte));
      } else lcd.blink();   //once the screen is filled, prevent the user from adding any extra printable characters to the stack and block the display

    }
  }
}

bool isOperator(char value) {
  if (value == '(') return true;
  if (value == ')') return true;
  if (value == '^') return true;
  if (value == '/') return true;
  if (value == '*') return true;
  if (value == '+') return true;
  if (value == '-') return true;
  if (value == '.') return true;
  return false;
}

void errorCheck(byte returnvalue) {
  if (returnvalue == 0) Serial.println(F("SUCCESS"));
  if (returnvalue == 1) Serial.println(F("SYNTAX ERROR: NOT ENOUGH OPERANDS"));
  if (returnvalue == 2) Serial.println(F("SNYTAX ERROR: NOT ENOUGH OPERATORS"));
  if (returnvalue) {
    lcd.clear();
    lcd.print(F("SYNTAX ERROR"));
  }
}

byte calculate(QueueList <char> &inputQueue) {
  char infixRAW[900]; // max displayable characters on the lcd. (can be expanded if desired with a different lcd)
  memset(infixRAW, 0, strlen(infixRAW)); // clears array of garbage data and sets all elements to 0

  int rawindex = 0;
  bool wasNumber = false;

  StackList <char> opstack;
  QueueList <char> numberstack;
  StackList <BigNumber> resultQueue;

  resultQueue.setPrinter(Serial);

  while (!inputQueue.isEmpty()) {                           // =========== while inputqueue is not empty, loop everything between this and below ==============
    char character = inputQueue.pop();

    if (character == '(') {  // '(' has no precedence value, right parenth has only one expected behavior (below).
      opstack.push(character);
      if (wasNumber)saveNumber_push(resultQueue, numberstack, infixRAW, rawindex);

    } else if (isDigit(character) || character == '.') {   // if number or . then push to array (the array will eventually be converted into a BigNum)
      numberstack.push(character);
      wasNumber = true;
      //      push2Array(infixRAW, rawindex, character);
      //      wasNumber = true;

    } else if ( precedence(character) != NULL && precedence(character) != 5 ) {   // if operator and it is neither ( or ), time to open up code for operator logic


      if (wasNumber)saveNumber_push(resultQueue, numberstack, infixRAW, rawindex);

      while ( precedence(character) <= precedence(opstack.peek()) ) {
        if ( character == '^' && opstack.peek() == '^' ) {
          opstack.push(character);
          break;
        }
        doMath(resultQueue, opstack.pop());
      }  if ( (precedence(character) > precedence(opstack.peek())) ) opstack.push(character);


    } else if (character == ')') { // when we get closing parenthesis, empty opstack until '('


      if (wasNumber)saveNumber_push(resultQueue, numberstack, infixRAW, rawindex);

      while (opstack.peek() != '(') {
        doMath(resultQueue, opstack.pop());
      } if (opstack.peek() == '(') opstack.pop(); //discard the opening parenth

    }
  }                                                          // =========== while inputqueue is not empty, loop everything between this and above ==============

  Serial.println("initializing bignumber result to 0");
  BigNumber result = BigNumber(0);

  if (resultQueue.isEmpty()) return 1;
  else result = resultQueue.pop();
  if (!resultQueue.isEmpty()) return 2;   // if it's still not empty, then there is an extra operand that didnt recieve any operation, (NOT ENOUGH OPERATORS)
  else {
    Serial.print(F("RESULT: "));
    printBignum(result);                  // prints the result to serial monitor
    lcdprintBigNum(lcd, result);         // prints the result to LCD
    return 0;
  }
}

static byte precedence (char x) {
  if (x == '(') return NULL; // this is necessary
  else if (x == '+' || x == '-') return 1;
  else if (x == '*' || x == '/') return 2;
  else if (x == '^') return 3;
  else if (x == ')') return 5; // this too dont worry about it
  else return 5;
}

void push2Array(char z[], int &index, char input) {
  z[index] = input;
  Serial.println("value at z[" + String(index) + "] is: " + String(z[index]));
  index++;
  Serial.println("index++ makes the value now: " + String(index));
}

void saveNumber_push(StackList <BigNumber> &resultStack, QueueList <char> &numberstack, char x[], int &index) {
  // if there wasnt a number detected before, exit function immediately to prevent duplicate saving.
  //  BigNumber i = BigNumber(0);  //appears redundant, but it is necessary
  //  i = BigNumber
  Serial.println("savenumber_push: saved number as: V");
  Serial.println("!: ");
  //  printBignum();
  if (numberstack.isEmpty()) {
    Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!DANGER");
    return;
  }
  BigNumber i = BigNumber(byte(numberstack.pop()) - byte(48));
  while (!numberstack.isEmpty()) {
    i = i * BigNumber(10) + BigNumber(byte(numberstack.pop()) - byte(48));
  }
  printBignum(i);
  resultStack.push(i);
  index = 0;

  //    memset(x, 0, strlen(x)); // safely erases the BigNumber from memory (the library is great, but terrible for memory management, this whole thing being in arduino doesnt help either)
}

void doMath(StackList <BigNumber> &x, char operation) {
  if (!x.isEmpty()) {
    BigNumber y = x.pop();
    if (!x.isEmpty()) {
      BigNumber z = x.pop();
      if (operation == '+') z += y;
      if (operation == '-') z -= y;
      if (operation == '*') z *= y;
      if (operation == '/') z /= y;
      if (operation == '^') z = z.pow(y);
      x.push(z);
    } else Serial.println(F("SYNTAX ERROR: NOT ENOUGH OPERANDS"));
  }
}

byte convertBigNumber(BigNumber& n, int& i) {
  i = 0;
  BigNumber base10 = "10";

  if (n >= base10) {
    while (n >= base10) {   //converts BigNumber into scientific notation for easy printing (makes my life easier)
      wdt_reset();
      n /= base10;          //takes exponentially longer with bigger numbers (noticeable around 2^1004+ range  (2^2999 takes a minute to print)
      i++;
    }
  }

  if (n < BigNumber(1)) {
    while (n < BigNumber(1)) {
      n *= base10;
      i--;
    }
  } else return 1;

}

void lcdprintBigNum(LiquidCrystal_I2C & lcd, BigNumber & n) {
  byte t = 0;
  int i;
  Serial.println("STAGE 5");
  if (n.isNegative()) {
    n *= BigNumber(-1);
    t = 1;
  }
  convertBigNumber(n, i);

  lcd.setCursor(0, 1);
  lcd.print(F("RESULT:"));
  lcd.setCursor(0, 2);

  char *s = n.toString();
  if (t)lcd.print(F("-"));
  lcd.print(s);
  free(s);

  lcd.print(F("*10^"));
  lcd.print(i);

  lcd.home();
  t = 0;
}

void lcdsmartClear(LiquidCrystal_I2C & lcd, byte code) {
  if (!code) {
    x = 0;
    y = 0;
  } else {
    x = 0;
    y = 0;
    lcd.clear();
    lcd.home();
    lcd.noBlink();
    lcd.cursor();
  }
}

void lcdsmartPrint(LiquidCrystal_I2C & lcd, char input) {
  lcd.print(input);
  x++;
  if (x == 20) {                   // allows proper linewrap on lcd
    x = 0;
    y++;
  }
  lcd.setCursor (x, y);            // if cursor position isnt explicitly controlled, it overflows and skips lines on the display
}

void printStack(QueueList <char> &x) {
  Serial.print(F("STACK:"));
  while (!x.isEmpty()) Serial.print(x.pop());
  Serial.println("");
}

void printBignum (BigNumber & n) {
  char * s = n.toString ();
  Serial.println (s);
  free (s);
}
