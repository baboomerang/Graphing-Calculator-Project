// Calculator Peripheral Logic Board - Atmega328p
/* Designed to relay information between the attiny84 keypad serial and the Atmega2560
   This also includes the necessary io for processing buttons and status LED's
   This project consist of mostly aesthetic and probably unwise design choices, but this calculator is for myself anyways.


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
#define SPECIAL_PIN 12

//
//const byte LEFTPARENTH_PIN = 14;
//const byte RIGHTPARENTH_PIN = 15;
//const byte GRAPH_PIN = 16;

float brightness = 0;
float fadeAmount = 0.01;
bool isBusy = false;

LEDFader input_led = LEDFader(INPUT_PIN);

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
  pinMode(LED_PIN, OUTPUT);


  //  input_led.fade(255, 500);
}

void loop() {
  fadeLED(LED_PIN);
  if (isBusy) new_fadeLED();

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

  //  Serial.print(value6);
  //  Serial.print("value6        ");
  //  Serial.print(value7);
  //  Serial.println("  value7");

  if (value1) {
    if (debouncer1.rose()) {
      Serial.write(byte('+'));
    }
    bool ledState = HIGH;
    digitalWrite(INPUT_PIN, ledState);
    isBusy = true;
  } else if (debouncer1.fell()) {
    bool ledState = LOW;
    digitalWrite(INPUT_PIN, ledState);
  }

  if (value2) {
    if (debouncer2.rose()) {
      Serial.write(byte('-'));
    }
    bool ledState = HIGH;
    isBusy = false;
    digitalWrite(INPUT_PIN, ledState);
  } else if (debouncer2.fell()) {
    bool ledState = LOW;
    digitalWrite(INPUT_PIN, ledState);
  }

  if (value3) {
    if (debouncer3.rose()) {
      Serial.write(byte('*'));
    }
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

void fadeLED(byte pin) {
  brightness += fadeAmount;
  analogWrite(pin, brightness);
  if (brightness <= 0 || brightness >= 230) {
    fadeAmount = -fadeAmount;
  }
}

void new_fadeLED() {
  input_led.update();

  if (input_led.is_fading() == false) {

    // Fade from 255 - 0
    if (input_led.get_value() == 255) {
      input_led.fade(0, 25);
    }
    // Fade from 0 - 255
    else {
      input_led.fade(255, 80);
    }
  }
}
