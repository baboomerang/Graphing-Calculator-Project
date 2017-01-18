#include <Arduino.h>
#include <MemoryFree.h>

#include "BigNumber.h"
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Wire.h>
#include <Bounce2.h>

#define I2C_ADDR 0x27
#define BACKLIGHT_PIN 3
#define En_pin 2
#define Rw_pin 1
#define Rs_pin 0
#define D4_pin 4
#define D5_pin 5
#define D6_pin 6
#define D7_pin 7

#define BUTTON_PIN_MULT 5
#define BUTTON_PIN_DIV 6
#define BUTTON_PIN_DECIM 7
#define BUTTON_PIN_PI 8
#define BUTTON_PIN_NEG 5

Bounce debouncer = Bounce();
Bounce debouncer2 = Bounce();
Bounce debouncer3 = Bounce();
Bounce debouncer4 = Bounce();
Bounce debouncer5 = Bounce();

bool oldmultvalue = LOW;
bool olddivvalue = LOW;
bool olddecimvalue = LOW;
bool oldpivalue = LOW;
bool oldnegvalue = LOW;

byte pioverride;
bool main_override = false;

byte decimal = 0;
bool complete = 0;

byte looponce = 0;
byte zero = 0;
byte notzero = 0;
//boolean useless = false;
bool startprogmem = 0;

LiquidCrystal_I2C lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);

byte buttonprocessed = 0;
byte incomingByte = 0;
char byteChar = 0;

BigNumber divideByBase10 = 1;
BigNumber bignumByte = 0;
BigNumber first = 0;
BigNumber second = 0;
BigNumber total = NULL;
BigNumber negate = -1;

void setup() {
  Serial.begin(9600);
  BigNumber::begin();        //                                                                                                          THIS IS WHERE THE BIGNUMBER LIBRARY BEGINS
  BigNumber::setScale(20);
  lcd.begin (20, 4, LCD_5x8DOTS);
  lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
  lcd.home();
  lcd.setBacklight(HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("CALCULTR PROGRM v5.0"));
  lcd.setCursor(0, 1);
  lcd.print(F("~Upstream.0xEFA2C15~"));
  lcd.setCursor(0, 2);
  lcd.print(F("BY: ILAN RODRIGUEZ"));
  lcd.setCursor(0, 3);
  lcd.print(F("Revision: 10/23/2016"));
  pinMode(BUTTON_PIN_MULT, INPUT_PULLUP);
  debouncer.attach(BUTTON_PIN_MULT);
  debouncer.interval(10); // interval in ms
  pinMode(BUTTON_PIN_DIV, INPUT_PULLUP);
  debouncer2.attach(BUTTON_PIN_DIV);
  debouncer2.interval(10); // interval in ms
  pinMode(BUTTON_PIN_DECIM, INPUT_PULLUP);
  debouncer3.attach(BUTTON_PIN_DECIM);
  debouncer3.interval(10); // interval in ms
  pinMode(BUTTON_PIN_PI, INPUT_PULLUP);
  debouncer4.attach(BUTTON_PIN_PI);
  debouncer4.interval(10); // interval in ms
  pinMode(BUTTON_PIN_NEG, INPUT_PULLUP);
  debouncer5.attach(BUTTON_PIN_NEG);
  debouncer5.interval(10); // interval in ms
}

void loop() {
  Serial.println(freeMemory());
  serialdataPull();
  Serial.println(freeMemory());
  calcProc();
  Serial.println(freeMemory());
  debouncer.update();
  Serial.println(freeMemory());
  debouncer2.update();
  Serial.println(freeMemory());
  debouncer3.update();
  Serial.println(freeMemory());
  div_debounce();
  Serial.println(freeMemory());
  mult_debounce();
  Serial.println(freeMemory());
  decim_debounce();
  Serial.println(freeMemory());
  pi_debounce();
  Serial.println(freeMemory());
  neg_debounce();
  Serial.println(freeMemory());
}

//=============================================================================================================

//=============================================================================================================

void serialdataPull() {
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    buttonprocessed = 1;
    byteChar = char(incomingByte);
  }
}

void mult_debounce() {
  debouncer.update();
  byte value = debouncer.read();
  if (value != oldmultvalue) { // if the current read was the same as before, dont do any of this other code until it changes
    if ( value == LOW ) {
    } else {
      byteChar = char(42);
      buttonprocessed = 1; // added this because thats what "serialdataPull" does when it recieves a press
    }
  } // end of the (IF value is != old value )
  oldmultvalue = value;
}

void div_debounce() {
  debouncer2.update();
  byte value2 = debouncer2.read();
  if (value2 != olddivvalue) { // if the current read was the same as before, dont do any of this other code until it changes
    if ( value2 == LOW ) {
    } else {
      byteChar = char(47);
      buttonprocessed = 1; // added this because thats what "serialdataPull" does when it recieves a press
    }
  } // end of the (IF value is != old value )
  olddivvalue = value2;
}

void decim_debounce() {
  debouncer3.update();
  byte value3 = debouncer3.read();
  if (value3 != olddecimvalue) { // if the current read was the same as before, dont do any of this other code until it changes
    if ( value3 == LOW ) {
    } else {
      byteChar = char(46);
      buttonprocessed = 1; // added this because thats what "serialdataPull" does when it recieves a press
    }
  } // end of the (IF value is != old value )
  olddecimvalue = value3;
}

void pi_debounce() {
  debouncer4.update();
  byte value4 = debouncer4.read();
  if (value4 != oldpivalue) { // if the current read was the same as before, dont do any of this other code until it changes
    if ( value4 == LOW ) {
    } else {
      byteChar = char(112);
      buttonprocessed = 1; // added this because thats what "serialdataPull" does when it recieves a press
    }
  } // end of the (IF value is != old value )
  oldpivalue = value4;
}

void neg_debounce() {
  debouncer5.update();
  byte value5 = debouncer5.read();
  if (value5 != oldnegvalue) { // if the current read was the same as before, dont do any of this other code until it changes
    if ( value5 == LOW ) {
    } else {
      byteChar = char(95);
      buttonprocessed = 1; // added this because thats what "serialdataPull" does when it recieves a press
    }
  } // end of the (IF value is != old value )
  oldnegvalue = value5;
}

void calcProc() {  //===============================================================================================================================================================================================================================
  if (buttonprocessed == 1) {
    //    Serial.print("BUTTONPRESSEDfreeMemory()=");
    //    Serial.println(freeMemory());
    //    delay(100);
    switch (byteChar) {
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      case '0' ... '9' : { // This keeps collecting the first value until a operator is pressed "+-*/"
          BigNumber(bignumByte) = BigNumber(incomingByte) - BigNumber(48);
          //          lcd.setCursor(0, 0);
          if (!decimal) {
            first = first * BigNumber(10) + BigNumber(bignumByte);
          } else {
            //----------------------------------------------------------------------------------------------------------
            /* You cannot use the built in power function with C. As it updates power result,
                it memallocs again under BigNumber. Since the nature of the bignumber library is
                having everyuthing in the ram, you will corrupt and get random values.
                The Nature of the randomness is consistent with the number you press on the keypad.
                Fix: MAKE YOUR OWN INVERSE EXPONENT FUNCTION
            */
            //          divideByBase10 = BigNumber(pow(BigNumber(10), loops));
            //----------------------------------------------------------------------------------------------------------
            divideByBase10 = divideByBase10 * BigNumber(10);
            first = first + BigNumber(bignumByte) / BigNumber(divideByBase10);
          }
          if (complete == 1) {
            lcd.clear();
            lcd.home();
            lcd.blink();
            complete = 0;
          }
          printBignum(first, 1, false);
          break;
        }
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      case '.':
        if (startprogmem) {
          if (!decimal) {
            decimal = 1;
            lcd.print('.');
            //decimchar = char('.');
          }
        }
        break;

      case '_':
        if (startprogmem) {
          lcd.setCursor(0, 0);
          lcd.print(F("                    "));
          lcd.setCursor(0, 0);
          first = first * BigNumber(negate);
          printBignum(first, 1, false);
        }
        break;

      case 'p':
        if (startprogmem) {
          BigNumber(pi) = BigNumber("3.141592653589793238");
          if (first == 0) {
            //          Serial.println("first == 0 and we detected pi once");
            lcd.setCursor(0, 0);
            //          Serial.println("we have for bignum pi" + String(pi));
            //          String pistring = pi.toString();
            //          Serial.println(pistring);
            //          decimal = 1;
            first = pi;
            lcd.print(char(247));
          } else if ( pioverride == 0 ) {
            pioverride++;
            lcd.setCursor(0, 1);
            lcd.print(F("Pressing overrides"));
            lcd.setCursor(0, 2);
            lcd.print(F("the non-zero #"));
          } else if ( pioverride == 1 ) {
            lcd.clear();
            lcd.home();
            lcd.blink();
            BigNumber &first = pi;
            lcd.print(char(247));
            pioverride = 0;
          }
        }
        break;
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      case '+':
        if (startprogmem) {
          operatorProcess(byteChar);
          //        if (startprogmem) {
          //          looponce = 0;
          //          decimal = 0;
          //          divideByBase10 = 1;
          //          first = (total != 0 ? total : first);
          //          result_as_input();
          //          lcd.setCursor(0, 1);
          //          lcd.print("+");
          //          second = SecondNumber(); // // switch to secondNumber to detect/input the addend ( augend is combined with the addend )
          //        }
          total = first + second;
          reportResult();
        }
        break;
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      case '-':
        if (startprogmem) {
          operatorProcess(byteChar);
          //        if (startprogmem) {
          //          decimal = 0;
          //          looponce = 0;
          //          divideByBase10 = 1;
          //          first = (total != 0 ? total : first);
          //          result_as_input();
          //          lcd.setCursor(0, 1);
          //          lcd.print("-");                                                                     //  8         -             3   = 5
          //          second = SecondNumber(); // switch to secondNumber to detect/input the subtrahend ( minuend is reduced by the subtrahend )
          //        }
          total = first - second;
          reportResult();
        }
        break;
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      case '*':
        if (startprogmem) {
          operatorProcess(byteChar);
          //        if (startprogmem) {
          //          looponce = 0;
          //          decimal = 0;
          //          divideByBase10 = 1;
          //          first = (total != 0 ? total : first);
          //          result_as_input();
          //          lcd.setCursor(0, 1);
          //          lcd.print("*");
          //          second = SecondNumber(); // switch to secondNumber to detect/input the multiplicand ( multiplier is copied n-times by the multiplicand )
          //        }
          total = first * second;
          reportResult();
        }
        break;
      case '^':
        if (startprogmem) {
          looponce = 0;
          decimal = 0;
          divideByBase10 = 1;
          first = (total != 0 ? total : first);
          result_as_input();
          lcd.setCursor(0, 1);
          lcd.print("^ to the pwr of: ");
          second = SecondNumber(); // switch to secondNumber to detect/input the multiplicand ( multiplier is copied n-times by the multiplicand )
          total = first.pow(second);
          reportResult();
        }
        break;
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      case '/':
        if (startprogmem) {
          operatorProcess(byteChar);
          //        if (startprogmem) {
          //          looponce = 0;
          //          decimal = 0;
          //          divideByBase10 = 1;
          //          first = (total != 0 ? total : first);
          //          result_as_input();
          //          lcd.setCursor(0, 1);
          //          lcd.print("/");
          //          second = SecondNumber(); // switch to secondNumber to detect/input the divisor ( dividend is divided by the divisor )
          //          //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
          //          //      second == 0 ? lcd.print("Invalid") : total = first / second;
          //          //      this is a reimplementation of the ternary operator ' ? ' because of this error
          //          //      operands to ?: have different types 'size_t {aka unsigned int}' and 'BigNumber'
          //          //    if else statement rewritten implementation
          //        }
          if (second == 0) {
            //            Serial.println("we messed up the second number value in decimal: " + String(second));
            lcd.setCursor(0, 3);
            lcd.print(F("Invalid:DivideByZero"));
            break;
          } else {
            total = first / second;
          }
          //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
          reportResult();
        }
        break;
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      case 'C':
        memoryclear();
        complete = 0;
        startprogmem = 1;
        break;
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    }
    buttonprocessed = 0;
  } //end of button loop
  buttonprocessed = 0;
}   //end of inputdetector

void operatorProcess(char oper8) {
  looponce = 0;
  decimal = 0;
  divideByBase10 = 1;
  first = (total != 0 ? total : first);
  result_as_input();
  lcd.setCursor(0, 1);
  if (oper8 == '*') lcd.print("*"); else doNothing();
  if (oper8 == '+') lcd.print("+"); else doNothing();
  if (oper8 == '-') lcd.print("-"); else doNothing();
  if (oper8 == '/') lcd.print("/"); else doNothing();
  second = SecondNumber(); // switch to secondNumber to detect/input the multiplicand ( multiplier is copied n-times by the multiplicand )
  //    if (oper8 == '*') (total = first * second); else doNothing();
  //    if (oper8 == '+') (total = first + second); else doNothing();
  //    if (oper8 == '-') (total = first - second); else doNothing();
  //    if (oper8 == '/') { //1
  //      if (second != 0) { // 2
  //        (total = first / second);
  //        reportResult();
  //      } else { //close 2 open 3
  //        lcd.setCursor(0, 3);
  //        lcd.print(F("Invalid:DivideByZero"));
  //      }//close 3
  //    }//close 1
}//close operatorProcess

void doNothing() {
  int nothing;
  //  free(nothing);
}
// the key problem with second number() freezing out on:
/*is second() even returned?
  is second() even returned?
  is second() even returned?
  is second() even returned?
  is second() even returned?
  is second() even returned?
  is second() even returned?
  is second() even returned?
  is second() even returned?
  is second() even returned?
*/
// is that the loop within SecondNumber does not allow to go back to the input detector code from the loop. the 'While ( 1 ) ' doesnt allow break out into the 'input detector();'
BigNumber SecondNumber() {

  while ( 1 ) {
    decim_debounce();
    serialdataPull();
    if (buttonprocessed == 1 ) { // only check through this if else system once ( when the button is pressed once )
      //--------------------------------------------------------------------
      if (byteChar >= '0' && byteChar <= '9') {
        bignumByte = BigNumber(incomingByte) - BigNumber(48);
        if (!decimal) {
          second = second * BigNumber(10) + BigNumber(bignumByte);
        } else {
          divideByBase10 = divideByBase10 * BigNumber(10);
          second = second + BigNumber(bignumByte) / BigNumber(divideByBase10);
        }
        //        lcd.setCursor(0, 2);
        printBignum(second, 2, false);
      }
      if (byteChar == 'p') {
        BigNumber(pi) = BigNumber("3.141592653589793238");
        if (second == 0) {
          //          Serial.println("first == 0 and we detected pi once");
          lcd.setCursor(0, 2);
          //          Serial.println("we have for bignum pi" + String(pi));
          //          String pistring = pi.toString();
          //          Serial.println(pistring);
          //          decimal = 1;
          BigNumber& second = pi;
          lcd.print(char(247));
        } else if ( pioverride == 0 ) {
          pioverride++;
          lcd.setCursor(0, 3);
          lcd.print(F("Pressing overrides"));
          //          delay(50);
          lcd.setCursor(0, 3);
          lcd.print(F("the non-zero #"));
        } else if ( pioverride == 1 ) {
          lcd.setCursor(0, 3);
          lcd.print(F("                   "));
          lcd.print(char(247));
          pioverride = 0;
        }
      }
      //--------------------------------------------------------------------
      if (byteChar == '=') {
        break; //return second;
      }
      if (byteChar == '_') {
        lcd.setCursor(0, 2);
        lcd.print(F("                    "));
        lcd.setCursor(0, 2);
        second = second * BigNumber(negate);
        printBignum(second, 2, false);
      }
      //--------------------------------------------------------------------
      if (byteChar == '.') {
        if (!decimal) {
          decimal = 1;
          lcd.print('.');
        }
      }
      //--------------------------------------------------------------------
      if (byteChar == 'C') {
        memoryclear();
        break; //return second;
      }
      //--------------------------------------------------------------------
      buttonprocessed = 0;
    } // end of IF BUTTON PROCESSED = 1 ; ANYTHING IN BETWEEN WILL BE CHECKED FOR AS A CONDITION TO EXECUTE. USE IT AS YOU WISH.
  }
  return second;
} //end of secondnumber
// ========================================================================================================================================================================================================================================

// ========================================================================================================================================================================================================================================

// ========================================================================================================================================================================================================================================
//i have lost track of what the code for printing bignum does. It's so fucking confusing. Any single change on any line below WILL REQUIRE extensive testing through trial and error to fix.
// ========================================================================================================================================================================================================================================

// ========================================================================================================================================================================================================================================
void printBignum (BigNumber n, byte arith_part, bool isTotal) {
  main_override = false;
  int p;
  //  Serial.print("BEFORE LCDPRINTING BIGNUM freeMemory()=");
  //  Serial.println(freeMemory());
  bool startcounting = false;
  char * s = n.toString ();
  //  Serial.println (s);
  for (int i = 0; i < (strlen(s)); i++) {
    char *z = s[i];
    if (i < (strlen(s) - 21)) {
      if (arith_part == 1) {
        lcd.setCursor(i, 0);
        lcd.print(s[i]);
      } else if (arith_part == 2) {
        lcd.setCursor(i, 2);
        lcd.print(s[i]);
      } else if (arith_part == 3) {
        lcd.setCursor(i, 3);
        lcd.print(s[i]);
      }
    }
    // changed the beginning because it makes no effect on the system. Just make an exception if you are printing total, or printing first as the total, bypass this check completly with a boolean
    if (isTotal && isPunct(s[i])) startcounting = true; else if (isPunct(s[i]) && !decimal && !isTotal) break; else if (isPunct(s[i]) && decimal) startcounting = true;

    if (startcounting == true) {
      //      Serial.println("=============START COUNTING IS TRUE=========");
      s[i] != '0' ? checkednotzero() : zero++;
      main_override ? p = 2 : lcd.print(s[i]);
      //      Serial.println("notzero : " + String(notzero) + " zero" + String(zero));
      if (notzero >= 1 && zero >= 1) {
        //        Serial.println("WE GET THE S I PRINTED TO BLANK HOPEFULLY IN THE CODE");
        s[i] != '0' ? lcd.print(s[i]) : main_override = true;
        //        delay(50);
      } else if (zero > 8 && notzero == 1) {
        //        Serial.println("zero > 8 && notzero == 0");
      }
      //      if (notzero < 1 && zero < 5) {
      //        lcd.print(s[i]);
      //      } else if (zero < 8 && notzero != 0) {
      //        lcd.print(s[i]);
      //      } else {
      //        Serial.println("SO NOT ZERO IS : " + String(notzero) + "  AND ZERO IS " + String(zero)) ;
      //      }
    }//end of start counting

    //    if (notzero >= 1 && zero == 5) {
    //      Serial.println("WE BREAK");
    //      //      s[i] != '0' ? lcd.print(s[i]) : Serial.println("IGNORE DIS");
    //    } else if (zero >= 8 && notzero == 0) {
    //      if (arith_part == 1) {
    //        lcd.setCursor((strlen(s) - 21), 0);
    //        lcd.print("ZZ");
    //        Serial.println("WIPE CODE 1");
    //        break;
    //      }
    //      if (arith_part == 2) {
    //        lcd.setCursor((strlen(s) - 21), 1);
    //        lcd.print("YY ");
    //        Serial.println("WIPE CODE 2");
    //        break;
    //      }
    //      if (arith_part == 3) {
    //        lcd.setCursor((strlen(s) - 21), 3);
    //        lcd.print("XX ");
    //        Serial.println("WIPE CODE 3");
    //        break;
    //      }
    //    }
    //    else {
    //      Serial.println("WE WTF?");
    //      lcd.print(s[i]);
    //    }
  }
  free(s);
}

// ========================================================================================================================================================================================================================================

// ========================================================================================================================================================================================================================================

// ========================================================================================================================================================================================================================================

// ========================================================================================================================================================================================================================================
//( zero == 4 && notzero == 1 ) ? cutHere = v - 2  : ( zero >= 8 && notzero == 0 ) ? v = thedecim : cutHere = predecimlength;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//void lcdprintBignum (BigNumber zt) {
//  Serial.print("BEFORE LCDPRINTING BIGNUM freeMemory()=");
//  Serial.println(freeMemory());
//  //  delay(100);
//  String istring = zt.toString();
//  byte thedecim = istring.indexOf('.');
//  //========================================================================================================
//  if ( (zt == first || zt == second) && zt != total) {
//    String nondecstring = istring.substring(0, thedecim);
//    nondecstring.reserve(thedecim);
//    lcd.print (nondecstring);
//    if (decimal == 1) {
//      if (buttonprocessed) {
//        looponce < thedecim ? looponce = thedecim + 1 : looponce++;
//        String decstring = istring.substring(thedecim, looponce + 1);
//        decstring.reserve(20 - thedecim);
//        lcd.print(decstring);
//      }
//    }
//  }
//  //========================================================================================================
//  if ( zt == total && zt != second) {
//    zero = 0;
//    notzero = 0;
//    //    Serial.println("total = " + String(total) + "  first = " + String(first) + "  second detected AFTER loop = " + String(second));
//    byte cutHere;
//    String predecimal = istring.substring(0, thedecim);
//    byte predecimlength = predecimal.length();
//    //    predecimal.reserve(predecimlength);
//    //    Serial.println("here is what we have for predecimal   " + predecimal);
//    for (byte H = thedecim + 1; H < istring.length(); H++) {
//      byte z = istring.charAt(H) - 48;
//      //      Serial.println("character detected as  " + String(z));
//      z != 0 ? checkednotzero() : zero++;
//      byte v = H;
//      //      Serial.println("We Have Zero as: " + String(zero) + "  We Have NonZero As: " + String(notzero));
//      //      Serial.println("v is set to H and that is : " + String(v));
//      // in the middle parenthesis, we can enable an accuracy toggle and make sure that we still output the tiny decimal places.
//      // if switch on, dont cut off any extra decimal regardless of the number: if thats teh case, YOU AHVE TO CODE DECIMAL SHORTHAND
//      //                                                        V ==-- here
//      ( zero == 4 && notzero == 1 ) ? cutHere = v - 2  : ( zero >= 8 && notzero == 0 ) ? v = thedecim : cutHere = predecimlength;
//    }
//    //    Serial.println("TOTAL LENGTH AND PREDECIMAL LENGTH " + String(predecimlength));
//    //    Serial.println("Im checking the value of cutHere " + String(cutHere));
//    ( cutHere > 10  && predecimlength >= 10 ) ? cutHere = 20 - predecimlength : ( predecimlength < 10 ) ? cutHere = 20 - predecimlength : cutHere = 20 - predecimlength;
//    //    Serial.println("we escaped the loop meaning no logic error");
//    //    Serial.println("Im checking the value of cutHere " + String(cutHere));
//    //    delay(5);
//    String totalstring = istring.substring(0, cutHere + predecimlength);
//    //    totalstring.reserve(cutHere + predecimlength);
//    //    Serial.println(totalstring);
//    lcd.print(totalstring);
//    /* work on this thing so we can have vertical scrolling
//      the display code will be one of the most complex codes youll ever make*/
//    //    totalstring.length() > 19 ?
//    Serial.print("AFTER PRINTING BIGNUMfreeMemory()=");
//    Serial.println(freeMemory());
//
//    delay(100);
//  }//end of zt total
//  //========================================================================================================
//}// end of lcdprintbignum

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void memoryclear() {
  main_override = false;
  total = NULL;
  divideByBase10 = 1;
  decimal = 0;
  bignumByte = 0;
  first = 0;
  second = 0;
  total = NULL;
  lcd.clear();
  lcd.blink();
  second = 0;
}

void result_as_input() {
  if ( total != 0 ) {
    wipeDisplay();
    lcd.setCursor(0, 0);
    printBignum(first, 1, true);
  }
}

void wipeDisplay() {
  lcd.clear();
  lcd.blink();
}

void reportResult() {
  lcd.setCursor(0, 3);
  printBignum(total, 3, true);
  first = 0, second = 0; // reset values back to zero for next use
  complete = 1;
}

int checkednotzero() {
  notzero++;
  zero = 0;
}

