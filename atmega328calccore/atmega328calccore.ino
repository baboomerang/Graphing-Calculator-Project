#include <Arduino.h>


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
#define DIGITS 130

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

bc_num divideByBase10;
bc_num bignumByte = 0;
bc_num first = 0;
bc_num second = 0;
bc_num total = NULL;
//int pi = 0;
bc_num negate = 0;

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
  lcd.print(F("CALCULTR PROGRM v4.6"));
  lcd.setCursor(0, 1);
  lcd.print(F("~Heavily  Optimized~"));
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


  bc_add(divideByBase10, bc_num(1), &divideByBase10, DIGITS);
}

void loop() {
  serialdataPull();
  calcProc();
  debouncer.update();
  debouncer2.update();
  debouncer3.update();
  div_debounce();
  mult_debounce();
  decim_debounce();
  pi_debounce();
  neg_debounce();
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

void print_bignum (bc_num x)
{
  char *s = bc_num2str(x);
  Serial.println (s);
  free(s);
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
    switch (byteChar) {
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      case '0' ... '9' : { // This keeps collecting the first value until a operator is pressed "+-*/"
          bignumByte = bc_num(incomingByte - 48);
          lcd.setCursor(0, 0);
          if (!decimal) {
            bc_multiply(first, bc_num(10), &first, DIGITS);
            bc_add(first, bignumByte, &first, DIGITS);
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
            bc_multiply(divideByBase10, bc_num(10), &divideByBase10, DIGITS);
            first = bc_add(first, bc_divide(bignumByte, divideByBase10), &first, DIGITS);
          }
          if (complete == 1) {
            lcd.clear();
            lcd.home();
            lcd.blink();
            complete = 0;
          }
          //!@#$%^&*()_!@#$%^&*()_!~@#$%^&*()!@#$%^&*()  print the number here but make sure it doesnt overflow into the ram
          //          lcdprintBignum(first);
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
          first = bc_multiply(first, bc_mum(-1));
          //!@#$%^&*()_!@#$%^&*()_!~@#$%^&*()!@#$%^&*()  print the number here but make sure it doesnt overflow into the ram
          //          lcdprintBignum(first);
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
            BigNumber& first = pi;
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
          bc_add(first, second, total);
          //          total = first + second;
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
          bc_subtract(first, second, &total);
          //          total = first - second;
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
          bc_multiply(first, second, &total, DIGITS);
          //          total = first * second;
          reportResult();
        }
        break;
      case '^':
        if (startprogmem) {
          looponce = 0;
          decimal = 0;
          bc_divide(dividebyBase10, divideByBase10, &divideByBase10, DIGITS);
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
            bc_divide(first, second, &total)
            //            total = first / second;
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
  bc_divide(dividebyBase10, divideByBase10, &divideByBase10, DIGITS);
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
        bignumByte = bc_num(incomingByte - 48);
        if (!decimal) {
          bc_multiply(second, bc_num(10), &second, DIGITS);
          bc_add(second, bc_num(bignumByte), &second, DIGITS);
        } else {
          bc_multiply(divideByBase10, bc_num(10), &divideByBase10, DIGITS);
          bc_add(second, bc_divide(bignumByte, divideByBase10,), &second, DIGITS);
        }
        lcd.setCursor(0, 2);

        //        lcdprintBignum(second);
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
        bc_multiply(second, negate, &second, DIGITS);
        //        lcdprintBignum(second);
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

//void lcdprintBignum (BigNumber zt) {
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
//    predecimal.reserve(predecimlength);
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
//    totalstring.reserve(cutHere + predecimlength);
//    //    Serial.println(totalstring);
//    lcd.print(totalstring);
//    /* work on this thing so we can have vertical scrolling
//      the display code will be one of the most complex codes youll ever make*/
//    //    totalstring.length() > 19 ?
//  }//end of zt total
//  //========================================================================================================
//}// end of lcdprintbignum

void memoryclear() {
  total = NULL;
  //  divideByBase10 = 1;
  bc_divide(dividebyBase10, divideByBase10, &divideByBase10, DIGITS);
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
    //    lcdprintBignum(first);
  }
}

void wipeDisplay() {
  lcd.clear();
  lcd.blink();
}

void reportResult() {
  lcd.setCursor(0, 3);
  //  lcdprintBignum(total);
  first = 0, second = 0; // reset values back to zero for next use
  complete = 1;
}

int checkednotzero() {
  //Serial.println("Checked Integer Was Not Zero ");
  notzero = 1;
  zero = 0;
}

