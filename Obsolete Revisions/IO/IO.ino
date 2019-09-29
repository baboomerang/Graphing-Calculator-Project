/* @file IO
  || @version 3.0
  || @date 04/04/2018
  || @author: Ilan Rodriguez
  || @credits to Thomas O. Fredericks, Jeremy Gillick for the libraries
  || #
*/

// Display and Aesthetic - Atmega328p
/* Designed to control the display
   This also includes the necessary io for processing buttons and status LED's
   This code is mostly aesthetic and probably unwise design choices, but this calculator is for myself anyways.
*/


#include <Bounce2.h>
#include <LEDFader.h>
#include <Curve.h>

#define ADDITION_PIN 5
#define SUBTRACTION_PIN 6
#define MULTIPLICATION_PIN 7
#define DIVISION_PIN 8
#define EXPONENT_PIN 4
#define LEFTPARENTH_PIN 14  //a0  13 + 1
#define RIGHTPARENTH_PIN 15 //a1  13 + 2
#define GRAPH_PIN 16 // a2 13 + 3

#define LED_PIN 9
#define INPUT_PIN 10
#define ERROR_PIN 11
#define SPECIAL_PIN 3

//
//const byte LEFTPARENTH_PIN = 14;
//const byte RIGHTPARENTH_PIN = 15;
//const byte GRAPH_PIN = 16;

//float brightness = 0;
//float fadeAmount = 0.01;

bool isBusy = false;
bool isError = false;
bool isSpecial = false;

LEDFader power_led = LEDFader(LED_PIN);
LEDFader input_led = LEDFader(INPUT_PIN);
LEDFader special_led = LEDFader(SPECIAL_PIN);
LEDFader error_led = LEDFader(ERROR_PIN);

// Instantiate a Bounce object
Bounce debouncer1 = Bounce();
Bounce debouncer2 = Bounce();
Bounce debouncer3 = Bounce();
Bounce debouncer4 = Bounce();
Bounce debouncer5 = Bounce();
Bounce debouncer6 = Bounce();
Bounce debouncer7 = Bounce();
Bounce debouncer8 = Bounce();

void setup() {
  Serial.begin(9600);

  pinMode(ADDITION_PIN, INPUT_PULLUP);
  debouncer1.attach(ADDITION_PIN);
  debouncer1.interval(5); // interval in ms

  pinMode(SUBTRACTION_PIN, INPUT_PULLUP);
  debouncer2.attach(SUBTRACTION_PIN);
  debouncer2.interval(5); // interval in ms

  pinMode(MULTIPLICATION_PIN, INPUT_PULLUP);
  debouncer3.attach(MULTIPLICATION_PIN);
  debouncer3.interval(5); // interval in ms

  pinMode(DIVISION_PIN, INPUT_PULLUP);
  debouncer4.attach(DIVISION_PIN);
  debouncer4.interval(5); // interval in ms

  pinMode(EXPONENT_PIN, INPUT_PULLUP);
  debouncer5.attach(EXPONENT_PIN);
  debouncer5.interval(5); // interval in ms

  pinMode(LEFTPARENTH_PIN, INPUT_PULLUP);
  debouncer6.attach(LEFTPARENTH_PIN);
  debouncer6.interval(5); // interval in ms

  pinMode(RIGHTPARENTH_PIN, INPUT_PULLUP);
  debouncer7.attach(RIGHTPARENTH_PIN);
  debouncer7.interval(5); // interval in ms

  pinMode(GRAPH_PIN, INPUT_PULLUP);
  debouncer8.attach(GRAPH_PIN);
  debouncer8.interval(5); // interval in ms

  pinMode(INPUT_PIN, OUTPUT);
  pinMode(ERROR_PIN, OUTPUT);
  pinMode(SPECIAL_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

}

void loop() {
  //  fadeLED(LED_PIN);
  new_fadeLED(power_led, 5000, 3000);

  if (isBusy) new_fadeLED(input_led, 27, 70); else digitalWrite(INPUT_PIN, LOW);
  if (isError) new_fadeLED(error_led, 13, 971); else digitalWrite(ERROR_PIN, LOW);
  if (isSpecial) new_fadeLED(special_led, 251, 1562); else digitalWrite(SPECIAL_PIN, LOW);

  // Update the Bounce instances :
  debouncer1.update();
  debouncer2.update();
  debouncer3.update();
  debouncer4.update();
  debouncer5.update();
  debouncer6.update();
  debouncer7.update();
  debouncer8.update();

  // Get the updated value :
  int value1 = debouncer1.read();
  int value2 = debouncer2.read();
  int value3 = debouncer3.read();
  int value4 = debouncer4.read();
  int value5 = debouncer5.read();
  int value6 = debouncer6.read();
  int value7 = debouncer7.read();
  int value8 = debouncer8.read();

  if (value1) {
    if (debouncer1.rose()) {
      Serial.write(byte('+'));
    }
    isBusy = true;
    bool ledState = HIGH;
    digitalWrite(INPUT_PIN, ledState);
  } else if (debouncer1.fell()) {
    bool ledState = LOW;
    digitalWrite(INPUT_PIN, ledState);
  }

  if (value2) {
    if (debouncer2.rose()) {
      Serial.write(byte('-'));
    }
    isBusy = false;
    bool ledState = HIGH;
    digitalWrite(INPUT_PIN, ledState);
  } else if (debouncer2.fell()) {
    bool ledState = LOW;
    digitalWrite(INPUT_PIN, ledState);
  }

  if (value3) {
    if (debouncer3.rose()) {
      Serial.write(byte('*'));
    }
    isError = true;
    bool ledState = HIGH;
    digitalWrite(INPUT_PIN, ledState);
  } else if (debouncer3.fell()) {
    bool ledState = LOW;
    digitalWrite(INPUT_PIN, ledState);
  }

  if (value4) {
    if (debouncer4.rose()) {
      Serial.write(byte('/'));
    }
    isError = false;
    bool ledState = HIGH;
    digitalWrite(INPUT_PIN, ledState);
  } else if (debouncer4.fell()) {
    bool ledState = LOW;
    digitalWrite(INPUT_PIN, ledState);
  }

  if (value5) {
    if (debouncer5.rose()) {
      Serial.write(byte('^'));
    }
    isSpecial = true;
    bool ledState = HIGH;
    digitalWrite(INPUT_PIN, ledState);
  } else if (debouncer5.fell()) {
    bool ledState = LOW;
    digitalWrite(INPUT_PIN, ledState);
  }

  if (value6) {
    if (debouncer6.rose()) {
      Serial.write(byte('('));
    }
    isSpecial = false;
    bool ledState = HIGH;
    digitalWrite(INPUT_PIN, ledState);
  } else if (debouncer6.fell()) {
    bool ledState = LOW;
    digitalWrite(INPUT_PIN, ledState);
  }

  if (value7) {
    if (debouncer7.rose()) {
      Serial.write(byte(')'));
    }
    bool ledState = HIGH;
    digitalWrite(INPUT_PIN, ledState);
  } else if (debouncer7.fell()) {
    bool ledState = LOW;
    digitalWrite(INPUT_PIN, ledState);
  }

  // THIS KEY WILL BE THE (ENTER) EQUIVALENT FOR THE MEGA
  // WHEN THIS KEY IS TRIGGERED, TOGGLE ISBUSY UNTIL THE MEGA RETURNS A COMPLETED VALUE
  // IF THE MEGA DOESNT, OR A WATCHDOG TIMER OCCURS, SAVE A VALUE TO EEPROM ABOUT THE ERROR, THEN ON REBOOT, WRITE IT FORM THE MEGA TO THIS CHIP


  /* to do:
      enter equivalent
      toggle isbusy
      check for returned status value
      response to returned value
  */
  if (value8) {
    if (debouncer8.rose()) {
      Serial.write(byte('g'));
    }
    bool ledState = HIGH;
    digitalWrite(INPUT_PIN, ledState);
  } else if (debouncer8.fell()) {
    bool ledState = LOW;
    digitalWrite(INPUT_PIN, ledState);
  }

}

//void fadeLED(byte pin) {
//  brightness += fadeAmount;
//  analogWrite(pin, brightness);
//  if (brightness <= 0 || brightness >= 230) {
//    fadeAmount = -fadeAmount;
//  }
//}

void new_fadeLED(LEDFader &led, double lighttime, double fadetime) {
  led.update();
  if (led.is_fading() == false) {
    // Fade from 255 - 0
    if (led.get_value() == 255) {
      led.fade(0, fadetime);
    }
    // Fade from 0 - 255
    else {
      led.fade(255, lighttime);
    }
  }
}
