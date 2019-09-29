

/* @file Calculator
  || @version 3.0
  || @date 09/28/2019
  || @author: Ilan Rodriguez
  || @credits to nickgammon, Efstathios Chatzikyriakidis, Bob101, & Gabriel for libraries.
  ||
  Sketch uses 14888 bytes (5%) of program storage space. Maximum is 253952 bytes.
  Global variables use 734 bytes (8%) of dynamic memory, leaving 7458 bytes for local variables. Maximum is 8192 bytes.
  #*/
// Calculator Core - Atmega2560
/* This is the core of the project. It does all of the magic.
   IT WORKS WITH OR WITHOUT AN LCD. Just upload & write an expression
   Check your serial monitor */
#include <Arduino.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <BigNumber.h>
#include <QueueList.h>
#include <StackList.h>
#include <EEPROM.h>
#include <Wire.h>

#include <LiquidCrystal_I2C.h>
#define I2C_ADDR 0x27
#define BACKLIGHT_PIN 3
#define En_pin 2
#define Rw_pin 1
#define Rs_pin 0

#include <TFT_HX8357.h>
#include <User_Setup.h>
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT_Macros.h>
//#include <SD.h>
//#include "Adafruit_HX8357.h"
//#include "SPI.h>
#include <Adafruit_SPITFT.h>

// These are 'flexible' lines that can be changed

#define TFT_CS 53
#define TFT_DC 48
#define TFT_RST 8 // RST can be set to -1 if you tie it to Arduino's reset
#define SD_CCS 42


#define LTBLUE    0xB6DF
#define LTTEAL    0xBF5F
#define LTGREEN   0xBFF7
#define LTCYAN    0xC7FF
#define LTRED     0xFD34
#define LTMAGENTA 0xFD5F
#define LTYELLOW  0xFFF8
#define LTORANGE  0xFE73
#define LTPINK    0xFDDF
#define LTPURPLE  0xCCFF
#define LTGREY    0xE71C

#define BLUE      0x001F
#define TEAL      0x0438
#define GREEN     0x07E0
#define CYAN      0x07FF
#define RED       0xF800
#define MAGENTA   0xF81F
#define YELLOW    0xFFE0
#define ORANGE    0xFC00
#define PINK      0xF81F
#define PURPLE    0x8010
#define GREY      0xC618
#define WHITE     0xFFFF
#define BLACK     0x0000

#define DKBLUE    0x000D
#define DKTEAL    0x020C
#define DKGREEN   0x03E0
#define DKCYAN    0x03EF
#define DKRED     0x6000
#define DKMAGENTA 0x8008
#define DKYELLOW  0x8400
#define DKORANGE  0x8200
#define DKPINK    0x9009
#define DKPURPLE  0x4010
#define DKGREY    0x4A49

#define ADJ_PIN A0

TFT_HX8357 tft = TFT_HX8357(TFT_CS, TFT_DC);
//#define D4_pin 4
//#define D5_pin 5
//#define D6_pin 6
//#define D7_pin 7

BigNumber InputX = NULL;
BigNumber InputY = NULL;

BigNumber xMIN = -30;
BigNumber xMAX = 30;
BigNumber yMIN = -30;
BigNumber yMAX = 30;
BigNumber plotfreq = 1;
BigNumber ox, oy;

//LiquidCrystal_I2C lcd(I2C_ADDR, 20, 4);
QueueList <char> serialQueue;
/* ======================================================================================
  Alright, so....

   Insert test expression into your arduino serial monitor: ((3*5325)^7/3412*16)-12+(1555/12-29421)^2

   Result should be this (in postfix notation): 3 5325*7^3412/16*12-155512/29421-2^+

   Numerical Result : 1245079343380762145465942456.417651751986453041552689  or 1.245079 * 10^27
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
void errorCheck(byte returnvalue) {
  if (returnvalue == 0) Serial.print(F("SUCCESS"));
  if (returnvalue == 1) Serial.print(F("GENERAL SYNTAX ERROR"));
  if (returnvalue == 2) Serial.print(F("SYNTAX ERROR: NOT ENOUGH OPERANDS"));
  if (returnvalue == 3) Serial.print(F("SYNTAX ERROR: NOT ENOUGH OPERATORS"));
//  if (returnvalue) {
//    lcd.clear();
//    lcd.print(F("SYNTAX ERROR"));
//  }
}
void setup() {
  wdt_disable(); // disable watchdog immediately to prevent any bugs or bootloops
  watchdogSetup();// starts the timer, subsequent copies of this function are required to prevent undesired automatic device rebooting
  BigNumber::begin(20);
  Serial.begin(57600);

  if (EEPROM.read(2) == 99) {
    Serial.println(F("MEM OVERFLOW: TOO LARGE NUMBER")); // Serial0 tells the connected computer what happened
//    lcd.print(F("OVERFLOW # TOO LARGE")); // Same as above, but prints to lcd
    EEPROM.write(2, 0);
  }

  serialQueue.push('(');
  serialQueue.pop();

  Serial.print("INPUT:");

//  lcd.begin(); //(20, 4, LCD_5x8DOTS);
//  lcd.backlight();
//  //lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
//  //lcd.setBacklight(HIGH);
//  //lcd.blink();
//  lcd.home();
//  lcd.cursor();

  tft.fillScreen(BLACK);
  tft.setRotation(1);
  //  display1 = true;
}
void loop() {
  wdt_reset();
  tft.fillScreen(BLACK);
  tft.setRotation(1);
  if (Serial.available() > 0 || Serial1.available() > 0) {
    if (serialQueue.isEmpty())serialQueue.push('(');
    byte incomingbyte;
    if (Serial.available() > 0 ) incomingbyte = Serial.read(); //recieve byte from computer serial
    if (char(incomingbyte) == 'g') {
      for (InputX = xMIN; InputX <= xMAX; InputX += plotfreq) errorCheck(calculate(serialQueue, 1));
    }
    if (char(incomingbyte) == '=') errorCheck(calculate(serialQueue, 0));
    if (char(incomingbyte) == 'c') printStack(serialQueue);
    if ( ( isDigit(char(incomingbyte)) || isOperator(char(incomingbyte)) ) ) {
      serialQueue.push(char(incomingbyte)); // anything that isn't a control character is pushed to the stack
      Serial.print(String(char(incomingbyte)));
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
static byte precedence (char x) {
  if (x == '(') return NULL; // this is necessary
  else if (x == '+' || x == '-') return 1;
  else if (x == '*' || x == '/') return 2;
  else if (x == '^') return 3;
  else if (x == ')') return 5; // this too dont worry about it
  else return 5;
}
void printStack(QueueList <char> &x) {
  Serial.println(F(""));
  Serial.print(F("CLEARED STACK CONTENT:"));
  while (!x.isEmpty()) Serial.print(x.pop());
  Serial.println(F(""));
  x.push('(');
  Serial.print("INPUT:");
}
void printBignum (BigNumber & n) {
  char * s = n.toString ();
  Serial.println(s);
  free (s);
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
byte calculate(QueueList <char> &inputQueue, byte intent) {
  inputQueue.push(')'); // caps the queue
  StackList <char> opstack;
  QueueList <char> numberQueue;
  StackList <BigNumber> resultQueue;
  bool wasNumber = false;
  while (!inputQueue.isEmpty()) {//================================================================================ while inputqueue is not empty, loop everything between this and below ==============
    char character = inputQueue.pop();
    if (character == '(') {  // '(' has no precedence value, right parenth has only one expected behavior (below).
      opstack.push(character);
      if (wasNumber)saveNumber(resultQueue, numberQueue);
    }
    //------------------------------------------------------------------------------
    else if (isDigit(character) || character == '.') {   // if number or . then push to array (the array will eventually be converted into a BigNum)
      numberQueue.push(character);
      wasNumber = true;
    }
    //------------------------------------------------------------------------------
    else if ( precedence(character) != NULL && precedence(character) != 5 ) {   // if operator and it is neither ( or ), time to open up code for operator logic
      if (wasNumber)saveNumber(resultQueue, numberQueue);
      while ( precedence(character) <= precedence(opstack.peek()) ) {
        if ( character == '^' && opstack.peek() == '^' ) {
          opstack.push(character);
          break;
        }
        doMath(resultQueue, opstack.pop());
      }
      if ( (precedence(character) > precedence(opstack.peek())) ) opstack.push(character);
    }
    //------------------------------------------------------------------------------
    else if (precedence(character) == 5 && character == ')') { // when we get closing parenthesis or end of stack, empty opstack until '(' or until the beginning of the queue
      if (wasNumber)saveNumber(resultQueue, numberQueue);
      while (opstack.peek() != '(') doMath(resultQueue, opstack.pop()); // it will recursively parse the equation until it hits an opening parenth (or until the beginning of the queue)
      if (opstack.peek() == '(') opstack.pop(); //discard the opening parenth
    }
    //------------------------------------------------------------------------------
  }// ============================================================================================================== while inputqueue is not empty, loop everything between this and above ==============
  BigNumber result = "0";
  if (resultQueue.isEmpty()) return 2;    // if the result queue is empty, then there were more operators left in the
  else result = resultQueue.pop();
  if (!resultQueue.isEmpty()) return 3;   // if it's still not empty, then there is an extra operand that didnt recieve any operation, (NOT ENOUGH OPERATORS)
  else {
    Serial.println(F(""));
    Serial.print(F("RESULT: "));
    if (intent) InputY = result;
    printBignum(result);                  // prints the result to serial monitor
    plot(tft, InputX, InputY, 45, 290, 420, 260, xMIN, xMAX, 5, yMIN, yMAX, 5, "X", "Y", DKBLUE, RED, LTMAGENTA, WHITE, BLACK);
    //plot(disp,   x  ,  y    , gx,  gy,  w ,  h , xlo ,  xhi, xinc, ylo, yhi, yinc,        title, title, graph colors....)
    //    plot(x, y, 0.01, -1, 15, 1, -10, 15, 2, "tan(abs((X-6)*(X-9)))", "X", "Y", DKBLUE, RED, LTMAGENTA, WHITE, BLACK);
    return 0;
  }
}
void saveNumber(StackList <BigNumber> &resultQueue, QueueList <char> &numberQueue) { //converts the char Queue into a single BigNumber
  //nickgammon provided a method of typecasting a character array into a BigNumber
  //but when that method is used dynamically and numerous times, it would result in a '0' being saved
  //This is painfully slower than every other method known to man. Blame the arduino for which this
  //method is the only reliable one.
  if (numberQueue.isEmpty())return;
  BigNumber i = "0";
  BigNumber baseTen = "10";
  BigNumber one = "1";
  while (!numberQueue.isEmpty() && numberQueue.peek() != '.') {
    i *= baseTen;
    i += BigNumber(byte(numberQueue.pop()) - byte(48));
  }
  if (numberQueue.peek() == '.') {
    numberQueue.pop();//discard the ' . '
    while (!numberQueue.isEmpty()) {
      if (numberQueue.peek() == '.') return;
      one *= baseTen;
      i += BigNumber(BigNumber(byte(numberQueue.pop()) - byte(48)) / one);
    }
  }
  resultQueue.push(i);
}
