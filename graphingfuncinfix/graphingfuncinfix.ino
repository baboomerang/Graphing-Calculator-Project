/*

  This program provides cartesian type graph function

  It requires and Arduino Mega (or UNO) and an Adafruit 3.5" TFT 320x480 + Touchscreen Breakout Board
  https://learn.adafruit.com/adafruit-3-5-color-320x480-tft-touchscreen-breakout/overview

  Adafruit libraries
  https://github.com/adafruit/Adafruit_HX8357_Library/archive/master.zip
  https://github.com/adafruit/Adafruit-GFX-Library/archive/master.zip

  optional touch screen libraries
  https://github.com/adafruit/Touch-Screen-Library/archive/master.zip

  Revisions
  rev     date        author      description
  1       12-24-2015  kasprzak    initial creation


  This pin setting will also operate the SD card

  Pin settings

  Arduino   device
  5V        Vin
  GND       GND
  A0
  A1
  A2         Y+ (for touch screen use)
  A3         X- (for touch screen use)
  A4
  A5
  1
  2
  3
  4         CCS (42 for mega)
  5
  6
  7         Y- (44 for mega)
  8         X+ (46 for mega)
  9         DC (48 on mega * change define)
  10        CS (53 for mega * change define)
  11        MOSI (51 for mega)
  12        MISO  (50 for mega)
  13        CLK (SCK) (52 for mega)
  44        Y- (for touch screen only)
  46        X+ (for touch screen only)
  48        DC
  SDA
  SLC

*/
#include "BigNumber.h"


#include <SPI.h>
#include <SD.h>
#include <math.h>
#include "Adafruit_HX8357.h"


// These are 'flexible' lines that can be changed

#define TFT_CS 53
#define TFT_DC 48
#define TFT_RST 8 // RST can be set to -1 if you tie it to Arduino's reset
#define SD_CCS 5


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

double a1, b1, c1, d1, r2, r1, vo, tempC, tempF, tempK;

Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC);

// this is the only external variable used by the graph
// it's a flat to draw the coordinate system only on the first pass
boolean display1 = true;
boolean display2 = true;
boolean display3 = true;
boolean display4 = true;
boolean display5 = true;
boolean display6 = true;
boolean display7 = true;
boolean display8 = true;
boolean display9 = true;
double ox , oy ;
//===========================================================
//input / cin variables
char byteChar;

//Cin input X location of infix expression (infX) printing from left to right
unsigned int infX = 0;
// saved number array position in order ie. numberStack_FINAL[3] = "2323212" where num_indx = 3
unsigned int num_indx = 0;
// number delimiter based by operator dependency
unsigned int num_x = 0;
//infix reference stack X location
unsigned int infix_key_x = 0;
//postfix reference X location
unsigned int numberrepeat = 0;

// initialize the offset to 0. everytime we perform an operation where 2 operands join into 1, we increment this number up once to make sure we have a proper location of math done.
byte delete_ones = 0;

//infixnumberstack and the Cin String
char infixRAWnumberStack[300];  //string of only the numbers together in a linear fashion. (makes it easy for cutting and sorting into the number stack)
char infixstring[300]; // infix string buffer from serial input
//processed stacks of information
byte infix_stack_reference[200]; // reference key showing INFIX notation of the expression in a simplified view
byte postfix_stack_reference[200]; // reference key showing POSTFIX notation of the expression in a simplified view
//byte postfix_stack_copy[100]; // copy of reference stack for processing
byte postfix_opstack[50]; // a stack used for rearranging operators to get them in PEMDAS order.
BigNumber numberStack_FINAL[50]; // where operands are stored by index nmbrstack_FINAL[16] = "2932.231153" for example.

/* adjust the sizes of these stacks accordingly to your device,
  all these calculations are done rather quickly and well, but be mindful of the amount of operators
  and minimum # of operands required for proper function of the infix stack and postfix stack.*/

//count of how many open parenthesis are in the expression
unsigned int openparenth_count = 0;
//this is an immediate operator to parenthesis checker. Prevents operators from performing arbitrary operations on non-existing operands.
bool active_parenth = false;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  pinMode(ADJ_PIN, INPUT);
  tft.begin(HX8357D);
  tft.fillScreen(BLACK);

  BigNumber::begin();  //                                           THIS IS WHERE THE BIGNUMBER LIBRARY BEGINS
  BigNumber::setScale(20);
  infixstring[0] = '(';
  infX = 1;

  //  tft.setRotation(2);
  //  a1 = 3.354016E-03 ;
  //  b1 = 2.569850E-04 ;
  //  c1 = 2.620131E-06 ;
  //  d1 = 6.383091E-08 ;


  BigNumber x, y;

  tft.setRotation(1);
  graph_Setup(x, y, 0.0123, -10, 15, 1, -10, 15, 1, "tan(abs((inputX - 6)*(inputX - 9)))", "X", "Y", DKBLUE, RED, LTMAGENTA, WHITE, BLACK);
  //
  //
  //  for (x = -60; x <= 60; x += 1) {
  //    y = tan(abs((x - 6) * (x - 9)));
  //    Graph(tft, x, y, 60, 290, 390, 260, -60, 60, 10, -60, 60, 10, "tan(abs(x-6)*(x-9)) Function", "x", "sin(x)", DKBLUE, RED, GREEN, WHITE, BLACK, display1);
  //
  //  }
  //  for (x = -60; x <= 60; x += 1) {
  //
  //    y = x * x;
  //    Graph(tft, x, y, 60, 290, 390, 260, -60, 60, 10, -60, 60, 10, "Parabolic Function", "x", "sin(x)", DKBLUE, RED, LTMAGENTA, WHITE, BLACK, display2);
  //
  //  }
  //  for (x = -60; x <= 60; x += 1) {
  //
  //    y = log(x * x);
  //    Graph(tft, x, y, 60, 290, 390, 260, -60, 60, 10, -60, 60, 10, "Inverted Log of X Squared", "x", "sin(x)", DKBLUE, RED, LTBLUE, WHITE, BLACK, display3);
  //
  //  }
  // delay(1000);
}

void loop() {
  if (Serial1.available()) {
    int inByte = Serial1.read();
    Serial.println("WE HAVE FOR SERIAL 1: " + String(inByte) + " " + char(inByte));
  }
  if (Serial.available()) {
    int inByte = Serial.read();
    Serial.println("WE HAVE FOR SERIAL 0: " + String(inByte) + " " + char(inByte));
  }
  infixdataPull();
}

/*

  function to draw a cartesian coordinate system and plot whatever data you want
  just pass x and y and the graph will be drawn

  huge arguement list
  &d name of your display object
  x = x data point
  y = y datapont
  gx = x graph num_indxation (lower left)
  gy = y graph num_indxation (lower left)
  w = width of graph
  h = height of graph
  xlo = lower bound of x axis
  xhi = upper bound of x asis
  xinc = division of x axis (distance not count)
  ylo = lower bound of y axis
  yhi = upper bound of y asis
  yinc = division of y axis (distance not count)
  title = title of graph
  xlabel = x asis label
  ylabel = y asis label
  gcolor = graph line colors
  acolor = axi ine colors
  pcolor = color of your plotted data
  tcolor = text color
  bcolor = background color
  &redraw = flag to redraw graph on fist call only
*/

void graph_Setup(double inputX, double inputY, double Plotfreq, double Xmin, double Xmax, double Xinc, double Ymin, double Ymax, double Yinc, String TheTitle, String Xlabels, String Ylabels, unsigned int gridCol, unsigned int axiCol, unsigned int funcCol, unsigned int txtcolor, unsigned int bcolor ) {
  int Xcorner = 45;
  int Ycorner = 290;
  for (inputX = Xmin; inputX <= Xmax; inputX += Plotfreq) {
    //    inputY = inputX * inputX + 6 * inputX - 9;
    inputY = tan(abs((inputX - 6) * (inputX - 9)));
    //    inputY = 20 * sin(inputX);
    Graph(tft, inputX, inputY, Xcorner, Ycorner, 420, 260, Xmin, Xmax, Xinc, Ymin, Ymax, Yinc, TheTitle, Xlabels, Ylabels, gridCol, axiCol, funcCol, txtcolor, bcolor, display1);
  }
}


void Graph(Adafruit_HX8357 & d, double x, double y, double gx, double gy, double w, double h, double xlo, double xhi, double xinc, double ylo, double yhi, double yinc, String title, String xlabel, String ylabel, unsigned int gcolor, unsigned int acolor, unsigned int pcolor, unsigned int tcolor, unsigned int bcolor, boolean & redraw) {

  double ydiv, xdiv;
  // initialize old x and old y in order to draw the first point of the graph
  // but save the transformed value
  // note my transform funcition is the same as the map function, except the map uses long and we need doubles
  //static double ox = (x - xlo) * ( w) / (xhi - xlo) + gx;
  //static double oy = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
  double i;
  double temp;
  int rot, newrot;

  if (redraw == true) {

    redraw = false;
    ox = (x - xlo) * ( w) / (xhi - xlo) + gx;
    oy = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
    // draw y scale
    for ( i = ylo; i <= yhi; i += yinc) {
      // compute the transform
      temp =  (i - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

      if (i == 0) {
        d.drawLine(gx, temp, gx + w, temp, acolor);
      }
      else {
        d.drawLine(gx, temp, gx + w, temp, gcolor);
      }

      d.setTextSize(1);
      d.setTextColor(tcolor, bcolor);
      d.setCursor(gx - 40, temp);
      // precision is default Arduino--this could really use some format control
      d.println(i);
    }
    // draw x scale
    for (i = xlo; i <= xhi; i += xinc) {

      // compute the transform

      temp =  (i - xlo) * ( w) / (xhi - xlo) + gx;
      if (i == 0) {
        d.drawLine(temp, gy, temp, gy - h, acolor);
      }
      else {
        d.drawLine(temp, gy, temp, gy - h, gcolor);
      }

      d.setTextSize(1);
      d.setTextColor(tcolor, bcolor);
      d.setCursor(temp, gy + 10);
      // precision is default Arduino--this could really use some format control
      d.println(i);
    }

    //now draw the labels
    d.setTextSize(2);
    d.setTextColor(tcolor, bcolor);
    d.setCursor(gx , gy - h - 30);
    d.println(title);

    d.setTextSize(1);
    d.setTextColor(acolor, bcolor);
    d.setCursor(gx , gy + 20);
    d.println(xlabel);

    d.setTextSize(1);
    d.setTextColor(acolor, bcolor);
    d.setCursor(gx - 30, gy - h - 10);
    d.println(ylabel);


  }

  //graph drawn now plot the data
  // the entire plotting code are these few lines...
  // recall that ox and oy are initialized as static above
  x =  (x - xlo) * ( w) / (xhi - xlo) + gx;
  y =  (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
  d.drawLine(ox, oy, x, y, pcolor);
  d.drawLine(ox, oy + 1, x, y + 1, pcolor);
  d.drawLine(ox, oy - 1, x, y - 1, pcolor);
  ox = x;
  oy = y;

}

/*
  End of graphing functioin
*/

void doNothing(int code) {
  if (code == 0) active_parenth = false;
}

void infixdataPull() {
  if (Serial1.available() > 0 || Serial2.available() > 0) { //make sure serial is open Cin
    int incomingByte;
    if (Serial1.available() > 0 ) incomingByte = Serial1.read();
    if (Serial2.available() > 0 ) incomingByte = Serial2.read();
    byteChar = char(incomingByte);
    byteChar != 'g' ? infixstring[infX] = byteChar : infixstring[infX - 1] = infixstring[infX - 1];
    infX += 1;
    //=============== INFIXPARSER ========
    infixproc();                       // it will loop infixproc infinitely. infixproc will continue with its own subroutines once the character "g" is detected.
    //====================================
  } //end of if serial available
} // end of serial data pull

void infixproc() {  // INFIX PROC DOES EXACTLY WHAT GETLINE DOES IN C++, but arduino for some absurd reason, does not have C++ STDL so I had to manually code it in
  if (byteChar == 'g') {
    unsigned int start = 0;
    unsigned int cutHere = 0;
    infixstring[strlen(infixstring)] = ')';
    //once we press g, it will start to process the string entirely. thanks for the tip angad!
    for (byte i = 0; i < strlen(infixstring); i++) { //from the starting point of the infix string until the end of the string.....
      char procChar = char(infixstring[i]);
      switch (procChar) {
        case '0' ... '9':
          infixRAWnumberStack[num_x] = procChar;  //  infix string of 345+96-22/7-999 will become rawnumberstack of 34596227999 and depending on the distance between the sequence of numbers to the nearest operator, we can delimit and "cut" each number out of this raw stack like a stencil.
          num_x += 1;
          break;
        case '(':
          openparenth_count += 1;
          save_op(6);
          break;
        case ')':
          //Serial.println("num_x : " + String(num_x) + " | " + "parenthcount : " + String(openparenth_count) + " | " + "cutHere : " + String(cutHere) + " | ");
          openparenth_count -= 1;
          active_parenth = true;
          if ( cutHere != num_x ) {
            cutHere = num_x;
            //Serial.println(F("DEBUG: save num INVOKED BY CLOSING PARENTHESIS"));
            save_num(start, cutHere, num_indx);
          }
          save_op(7); // this line IS VERY ORDER SENSITIVE, DO NOT CHANGE THE ORDER OTHERWISE STUFF BREAKS. THIS LINE HAS TO BE HERE
          if (openparenth_count == 0) {
            //Serial.println(F("PARENTH LOGIC DEBUG: PARENTH COUNT IS 0"));
            syntax_check(i); //=================================================== as a terrible design choice, this whole program continues by calling the syntax check soubroutine
          }
          break;
        case '^':
          //           if (cutHere == num_x) {
          //             //Serial.println("doubleoperatorerror");
          //             //Serial.println("aborting.......");
          //             abort();
          //           } else {
          cutHere = num_x;
          active_parenth == false ? save_num(start, cutHere, num_indx) : doNothing(0);
          save_op(8);
          start = cutHere;
          //           }
          break;
        case '-':
          //           if (cutHere == num_x) {
          //             //Serial.println("doubleoperatorerror");
          //             //Serial.println("aborting.......");
          //             abort();
          //           } else {
          cutHere = num_x;
          active_parenth == false ? save_num(start, cutHere, num_indx) : doNothing(0);
          save_op(2);
          start = cutHere;
          //           }
          break;
        case '+':
          //           if (cutHere == num_x) {
          //             //Serial.println("doubleoperatorerror");
          //             //Serial.println("aborting.......");
          //             abort();
          //           } else {
          cutHere = num_x;
          active_parenth == false ? save_num(start, cutHere, num_indx) : doNothing(0);
          save_op(3);
          start = cutHere;
          //           }
          break;
        case '*':
          //           if (cutHere == num_x) {
          //             //Serial.println("doubleoperatorerror");
          //             //Serial.println("aborting.......");
          //             abort();
          //           } else {
          cutHere = num_x;
          //          //Serial.println(F("SAVENUM INVOKED BY MULTIPLICATION"));
          active_parenth == false ? save_num(start, cutHere, num_indx) : doNothing(0);
          save_op(4);
          start = cutHere;
          //           }
          break;
        case '/':
          //           if (cutHere == num_x) {
          //             //Serial.println("doubleoperatorerror");
          //             //Serial.println("aborting.......");
          //             abort();
          //           } else {
          cutHere = num_x;
          active_parenth == false ? save_num(start, cutHere, num_indx) : doNothing(0);
          save_op(5);
          start = cutHere;
          //           }
          break;
      } // end of switch case
    } // end of for loop
    Serial.println("Recieved infix string: " + String(infixstring));

    for (int i = 0; i < (sizeof(infix_stack_reference) / sizeof(int)); i++) {
      Serial.print(String(infix_stack_reference[i]));
    } Serial.println("");

    evaluate_postfix();
  } //end of if character g
} //end of infixproc

void save_num(int start, int cutpoint, int index_xpos) {
  save_op(1);
  String Z = String(infixRAWnumberStack);
  String Zshort = Z.substring(start, cutpoint);
  //the cutpoint does not include the value at the cutpoint. its an exclusion limit. if we had 97806 and cut point was 4 digits with start 0, we would just get the first 3 (978)
  BigNumber i = BigNumber(Zshort.toInt());
  //  char * s = Zshort;
  //  BigNumber i = s;
  //  free(s);
  numberStack_FINAL[index_xpos] = i;
  num_indx++;
  //numberStack_FINAL IS WHERE ALL THE FULL NUMBERS ARE STORED.  numberStack_FINAL[x] = "23223" numberStack_FINAL[x+1] = "3567" .... etc
  //Serial.println("OUTPUT: we have this for Z / infixRAWnumberStack simplified " + String(Z) + "    OUTPUT: we have this for individual i.shortstring saved " + String(i));
}

void save_op(int reference_type) {
  /* 1 = number
     2 = subtraction
     3 = addition
     4 = multiplication
     5 = division
     6 = right facing parenthesis (
     7 = left facing parenthesis )
     8 = ... EXPONENT ?
     9? */
  infix_stack_reference[infix_key_x] = reference_type;
  infix_key_x++;
}

void syntax_check(int end_of_string) {
  if (end_of_string == (strlen(infixstring) - 1)) {
    openparenth_count != 0 ? abort() : calculate_postfix();
    //Serial.println(F("so we didnt abort either? NICE"));
  }
}

void calculate_postfix() {
  infix_key_x = 0;
  for ( int i = 1; i != 0; i = infix_stack_reference[infix_key_x] ) {
    i = infix_stack_reference[infix_key_x];   // REDUNDANCY CHECK, THIS IS REPETITIVE OF THE ACTION TO PERFORM ON EACH LOOP. ^^^^  sets it twice, make sure this line is needed or not just to simplify code.
    infix_key_x++;
    if ( i == 1 ) {
      copy(numberrepeat, 1);
      /* we know that the infix notation expression will always end with a 1,
        so by piggybacking on that predictable ending, we can tie together
        a final check to see if its actually the end of the string */
      //      if ( infix_stack_reference[infix_key_x + 1] == 0 ) { // WARNING!!!!!!!!!!!!!!!!! this code might potentially be problematic if we have infix expressions that end in parenthesis.
      //             ======== Operator Stack Popping Code
      //                for (int p = ((sizeof(postfix_opstack) - 1) / sizeof(int)) ; p >= 0 ; p--) {
      //                  int opr8tr = postfix_opstack[p];
      //                  if ( opr8tr != 0 ) { // this for loop essentially pops the stack from top to the bottom in descending order. if any alignment errors, the != 0 mediates any small calibraton issues
      //                    copy(numberrepeat, opr8tr);
      //                  }
      //                }
      //              =====================
      //      }
    } // end of if i == 1 so anything below this checkes for other types of reference numbers. *ie operators or modifier characters.
    if ( i == 2 || i == 3 ) {
      pushtostack(2, i);
    } else if ( i == 4 || i == 5 ) {
      pushtostack(3, i);
    } else if ( i == 6 ) {
      pushtostack(255, i);
    } else if ( i == 7) {
      pushtostack(255, i);
    } else if ( i == 8) {
      pushtostack(4, i);
    }
  }

  /* this prints the completed postfix_reference_stack once we finish pushing the last operator to the stack and pushtostack() exits */
  /*for ( int p = ((sizeof(postfix_stack_reference) - 1) / sizeof(int)) ; p >= 0 ; p--) {
    Serial.println("postfix_stack_reference[" + String(p) + "] =  " + String(postfix_stack_reference[p]));
    }*/

}

void print_opstack() {
  /*
    delay(50);
    Serial.print("Opstack is : ");
    for (int i = 0; ( i < (sizeof(postfix_opstack) / sizeof(int))); i++) {
    Serial.print(String(postfix_opstack[i]));
    }Serial.println("");
    delay(50);  */
}

void copy(byte location, byte input ) {
  postfix_stack_reference[location] = input;
  numberrepeat++;
}

void pushtostack(byte precedence, int opr8tr) {
  print_opstack();
  //Serial.println("pushtostack() DEBUG: precedence: " + String(precedence) + " operator value: " + String(opr8tr));
  //some funky stuff with checking p = 1 and it wont override it at all due to some funky stuff. Any value after p == 1 breaks the code chain and dupes the top op code for some stupid reason. go fix that bro
  for ( int p = ((sizeof(postfix_opstack) - 1) / sizeof(int)) ; p >= 0 ; p--) {

    if ( p == 0 && postfix_opstack[p] == 0 ) { //this would set the first operator into the stack considering its all 0's first and we have to make sure its the bottom one.
      postfix_opstack[p] = opr8tr;
      //      //Serial.println("location is  " + String(p) + " operator value: " + String(postfix_opstack[p]));
      //        |        FIRST CONDITION    |    |  ------>  all of the rest is second branch of the OR statement -------------------->                        '                                                                       '
    } else if ((precedence != 255) && (opr8tr < postfix_opstack[p] && postfix_opstack[p] != 6) || ( (precedence == 2 && (postfix_opstack[p] == 2 || postfix_opstack[p] == 3)) || (precedence == 3 && (postfix_opstack[p] == 4 || postfix_opstack[p] == 5))  )    )  {
      /* CODE EXPLANATION -
        So first, we initialize a local temp variable to store the value of the fualty operator
        then we copy that temp variable to the postfix reference stack (ie. "popping" the stack)
        Finally, the variable in that spot "postfix_opstack[p]" is replaced with the new opr8ter given to us by the "input". (pushtostack(precedence, input oper8ter))
        precedence is needed to generalize our input oper8tr and compare it to the actual operators already present in the opstack */
      int selected_oper8tr_in_opstack = postfix_opstack[p];
      copy(numberrepeat, selected_oper8tr_in_opstack);
      postfix_opstack[p] = opr8tr;
      for ( int loc = p + 1 ; loc < ((sizeof(postfix_opstack) - 1) / sizeof(int)); loc ++ ) postfix_opstack[loc] = 0;
    } else if ((opr8tr != 7 && opr8tr >= postfix_opstack[p] && postfix_opstack[p] != 0 && postfix_opstack[p] != 6))  {
      postfix_opstack[ p + 1 ] = opr8tr;
      break;
    } else if ((opr8tr != 7 && opr8tr == 8 && opr8tr >= postfix_opstack[p] && postfix_opstack[p] != 0))  {
      postfix_opstack[ p + 1 ] = opr8tr;
      break;
    } else if ((opr8tr != 7 && opr8tr <= postfix_opstack[p] && postfix_opstack[p] != 0 && postfix_opstack[p] == 6))  {
      postfix_opstack[ p + 1 ] = opr8tr;
      break;
    } else if (precedence == 255 && opr8tr == 7) {
      if (postfix_opstack[p] != 0) {
        //Serial.println("Checking the operator in the postfix_opstack[" + String(p) + "]   " + "we have : " + String(postfix_opstack[p]));
        int operator_2b_popped = postfix_opstack[p];
        postfix_opstack[p] = 0;
        if (operator_2b_popped != 6) {
          //          delay(100);
          //Serial.println("=======POPPED OPERATOR========= : " + String(operator_2b_popped));
          copy(numberrepeat, operator_2b_popped);
        } else break;
      }
    }
  }
}

void perform_operation(int input_operator, int pos) {
  int numberstack_index = -1;
  //Serial.println(delete_ones);
  for (int z = pos; z >= 0; z--) {
    if ( postfix_stack_reference[z] == 1 ) {
      numberstack_index++;
      //Serial.println("# of 1's detected when backprinting" + String(numberstack_index));
    }
  } numberstack_index -= delete_ones;
  if (input_operator == 2) {
    //Serial.println("first operand: " + String(numberStack_FINAL[numberstack_index - 1]) + " second operand: " + String(numberStack_FINAL[numberstack_index]) + " SUBRACTION");
    numberStack_FINAL[numberstack_index - 1] -= numberStack_FINAL[numberstack_index];
    //Serial.println("result" + String(numberStack_FINAL[numberstack_index - 1]));
    bring_stack_down(numberstack_index);
    //print_numberstack();
  }
  if (input_operator == 3) {
    //Serial.println("first operand: " + String(numberStack_FINAL[numberstack_index - 1]) + " second operand: " + String(numberStack_FINAL[numberstack_index]) + " ADDITION");
    numberStack_FINAL[numberstack_index - 1] += numberStack_FINAL[numberstack_index];
    //Serial.println("result" + String(numberStack_FINAL[numberstack_index - 1]));
    bring_stack_down(numberstack_index);
    //print_numberstack();
  }
  //  if (input_operator == 8) numberstack_FINAL[numberstack_index - 1] = numberstack_FINAL[numberstack_index - 1 ] exp numberstack_FINAL[numerstack_index];
  if (input_operator == 4) {
    //Serial.println("first operand: " + String(numberStack_FINAL[numberstack_index - 1]) + " second operand: " + String(numberStack_FINAL[numberstack_index]) + " MULTIPLYING");
    numberStack_FINAL[numberstack_index - 1] *= numberStack_FINAL[numberstack_index];
    //Serial.println("result" + String(numberStack_FINAL[numberstack_index - 1]));
    bring_stack_down(numberstack_index);
    //print_numberstack();
  }
  if (input_operator == 5) {
    //Serial.println("first operand: " + String(numberStack_FINAL[numberstack_index - 1]) + " second operand: " + String(numberStack_FINAL[numberstack_index]) + " DIVIDING");
    numberStack_FINAL[numberstack_index - 1] /= numberStack_FINAL[numberstack_index];
    //Serial.println("result" + String(numberStack_FINAL[numberstack_index - 1]));
    bring_stack_down(numberstack_index);
    //print_numberstack();
  }
  if (input_operator == 8) {
    //Serial.println("first operand: " + String(numberStack_FINAL[numberstack_index - 1]) + " second operand: " + String(numberStack_FINAL[numberstack_index]) + " EXPONENTIATION");
    numberStack_FINAL[numberstack_index - 1] = numberStack_FINAL[numberstack_index - 1].pow(numberStack_FINAL[numberstack_index]);
    //Serial.println("result" + String(numberStack_FINAL[numberstack_index - 1]));
    bring_stack_down(numberstack_index);
    //print_numberstack();
  }
}

void bring_stack_down(int pop_at_this_x) {
  for ( int m = pop_at_this_x ; m < (sizeof(numberStack_FINAL) / (sizeof(BigNumber))); m++) {
    //Serial.println("pop_at_this_x: " + String(m) + "  (sizeof(numberStack_FINAL/sizeof(BigNumber)):  " + String((sizeof(numberStack_FINAL) / (sizeof(BigNumber)))));
    //Serial.println("numberStack_FINAL[m]: " + String(numberStack_FINAL[m]) + " numberStack_FINAL[m + 1]: " + String(numberStack_FINAL[m + 1]));
    numberStack_FINAL[m] = numberStack_FINAL[m + 1];
  }
  delete_ones++;
}

void print_numberstack() {
  Serial.print("Numberstack: ");
  for ( int t = 0; t < (sizeof(numberStack_FINAL) / sizeof(BigNumber)); t++) {
    Serial.print(String(numberStack_FINAL[t]) + " ");
  } Serial.println(" ");
}


void evaluate_postfix() {
  //Serial.println("WE GOT IN TO EVALUATING POSTFIX");
  //boolean had_a_number = false;
  //print_numberstack();
  for ( int p = 0 ; p <= ((sizeof(postfix_stack_reference) - 1) / sizeof(int)) ; p++ ) {
    int value = postfix_stack_reference[p];
    //Serial.println("Position on Reference Stack : " + String(p) + "  Value: " + String(value));
    //value == 1 ? had_a_number = true : had_a_number == had_a_number;
    if (value > 1) {
      //Serial.println("Value greater than 1 at position: " + String(p));
      perform_operation(value, p);
      //had_a_number = false;
    }
  }

  Serial.print("Finished Processing, we got result approximate : ");
  Serial.println(numberStack_FINAL[0]);
  //    print_numberstack();

}


