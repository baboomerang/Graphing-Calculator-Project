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

byte buttonprocessed = 0;
char byteChar;

unsigned int num_x = 0;

char infixRAWnumberStack[100];
char infixstring[1000];
//char postfixstring[1000];

unsigned int infX = 0;

//bool graph = LOW;

long numberStack_FINAL[26];

char procChar;
unsigned int num_indx = 0;
unsigned int openparenth_count = 0;

unsigned int infix_key_x = 0;

int infix_stack_reference[50];
int postfix_opstack[50];

int postfix_stack_reference[50];
unsigned int numberrepeat = 0;
unsigned int location = 0;

// 1 = number
// 2 = sub
// 3 = add
// 4 = mult
// 5 = div
// 6 = other

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

void setup() {
  infixstring[0] = '(';
  infX = 1;
  Serial.begin(9600);
  pinMode(ADJ_PIN, INPUT);
  tft.begin(HX8357D);
  tft.fillScreen(BLACK);

  tft.setRotation(2);
  a1 = 3.354016E-03 ;
  b1 = 2.569850E-04 ;
  c1 = 2.620131E-06 ;
  d1 = 6.383091E-08 ;


  double x, y;

  tft.setRotation(1);
  graph_Setup(x, y, 0.7, -80, 80, 10, -80, 80, 10, "X-Cubed(glitchy)", "lol", "x?", DKBLUE, RED, LTMAGENTA, WHITE, BLACK);
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
  delay(1000);
}

void loop() {
  serialdataPull();
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
    inputY = tan(abs((inputX - 6) / (inputX + 9)));
    //    inputY = 20 * sin(inputX);
    Graph(tft, inputX, inputY, Xcorner, Ycorner, 420, 260, Xmin, Xmax, Xinc, Ymin, Ymax, Yinc, TheTitle, Xlabels, Ylabels, gridCol, axiCol, funcCol, txtcolor, bcolor, display1);
  }
}

void serialdataPull() {
  if (Serial.available() > 0) {
    int incomingByte = Serial.read();
    buttonprocessed = 1;
    byteChar = char(incomingByte);
    byteChar != 'g' ? infixstring[infX] = byteChar : infixstring[infX - 1] = infixstring[infX - 1];
    infX += 1;
    graphproc();
    Serial.println("pre - G - infix string: " + String(infixstring));
  }
}

void graphproc() {
  if (byteChar == 'g') {
    infixstring[strlen(infixstring)] = ')';
    unsigned int start = 0;
    unsigned int cutHere = 0;
    for (byte i = 0; i < strlen(infixstring); i++) {
      procChar = char(infixstring[i]);
      switch (procChar) {
        case '0' ... '9':
          infixRAWnumberStack[num_x] = procChar;
          num_x += 1;
          break;
        case '(':
          openparenth_count += 1;
          break;
        case ')':
          openparenth_count -= 1;
          cutHere = num_x;
          openparenth_count == 0 ? save_num(start, cutHere, num_indx) : abort();  //consider editing this abort() in the futre, why should we abort if we have additional parenthesis? assuming correct syntax?
          //test_index(); // DONT TAKE THIS OFF LOL
          syntax_check(i);
          break;
        case '-':
          if(cutHere == num_x) {
            Serial.println("doubleoperatorerror");
            Serial.println("aborting.......");
            abort();
          } else {
          cutHere = num_x;
          save_num(start, cutHere, num_indx);
          num_indx++;
          infix_stack_reference[infix_key_x] = 2;
          infix_key_x++;
          start = cutHere;
          syntax_check(i);
          }
          break;
        case '+':
          if(cutHere == num_x) {
            Serial.println("doubleoperatorerror");
            Serial.println("aborting.......");
            abort();
          } else {
          cutHere = num_x;
          save_num(start, cutHere, num_indx);
          num_indx++;
          infix_stack_reference[infix_key_x] = 3;
          infix_key_x++;
          start = cutHere;
          syntax_check(i);
          }
          break;
        case '*':
          if(cutHere == num_x) {
            Serial.println("doubleoperatorerror");
            Serial.println("aborting.......");
            abort();
          } else {
          /*COMPARE THE NUMX VALUE WHEN CALLED TWICE,
            IT STAYS THE SAME SO ADD A CHECKING CODE BLOCK FOR EACH OPERATOR CASE
            SO IF THE NUMX DOESNT CHANGE WHEN ANOTHER OPERATOR IS CALLED, (THERE WASNT A NUMBER INBETWEEN) IT HAS TO MEAN A DOUBLE OPERATOR ERROR
            BY THE USER                                 ALREADY ADDED 9:14 AM 2/27/17*/
          cutHere = num_x;
          save_num(start, cutHere, num_indx);
          num_indx++;
          infix_stack_reference[infix_key_x] = 4;
          infix_key_x++;
          start = cutHere;
          syntax_check(i);
          }
          break;
        case '/':
          if(cutHere == num_x) {
            Serial.println("doubleoperatorerror");
            Serial.println("aborting.......");
            abort();
          } else {
          cutHere = num_x;
          save_num(start, cutHere, num_indx);
          // save num also performs infix_key_x++  - just a tip and dont forget that
          //calling this function and successfully saving a number adds 1(meaning a number) to the reference stack but also ++ to the X location of that stack.
          //that x location variable is named infix_key_x and thus doesnt really complicate anything. Infact it just makes it more confusing because the infix_key_x is modified in different nested functions.
          num_indx++;
          infix_stack_reference[infix_key_x] = 5;
          infix_key_x++;
          start = cutHere;
          syntax_check(i);
          }
          break;
      }
    }
  }
}

//void test_index() {
//  for (int lol = 0; lol < (sizeof(numberStack_FINAL) / sizeof(long)); lol++) {
//    Serial.println(" So this is what we have for input index in " + String(lol) + " NUMBER:   " + String(numberStack_FINAL[lol]) );
//    if (numberStack_FINAL[lol] == 0) break;
//  }
//  for (int lol = 0; lol < ((sizeof(infix_stack_reference)) / sizeof(int)); lol++) {
//    Serial.println(" ~~~~~?@#!#!@$?!@$!@$!@#>!@3?!>@#   CALC PROC POSITION " + String(lol) + " TYPE STATUS:   " + String(infix_stack_reference[lol]) );
//    if (infix_stack_reference[lol] == 0) break;
//  }
//}

void save_num(int start, int cutpoint, int index_xpos) {
  infix_stack_reference[infix_key_x] = 1;
  infix_key_x++;
  String Z = String(infixRAWnumberStack);
  String Zshort = Z.substring(start, cutpoint);
  //the cutpoint does not include the value at the cutpoint. its an exclusion limit. if we had 35456 and cut point was 4 with start 0, we would just get the first 3
  long i = Zshort.toInt();
  numberStack_FINAL[index_xpos] = i;
  //numberStack_FINAL IS WHERE ALL THE FULL NUMBERS ARE STORED.  numberStack_FINAL[x] = "23223" numberStack_FINAL[x+1] = "3567" .... etc
  Serial.println("OUTPUT: we have this for Z / infixRAWnumberStack simplified " + String(Z) + "    OUTPUT: we have this for individual i.shortstring saved " + String(i));
}

void syntax_check(int end_of_string) {
  //  if (infix_stack_reference[infix_key_x] == infix_stack_reference[infix_key_x - 1] ) {
  //    Serial.println("MAJOR SYNTAX COMPLICATION, YOU HAVE A DOUBLE OPERATOR SOMEWHERE");
  //    //here you would write some abort code, or tell the user that the code has an extra operator
  //    abort(); NOT NECESSARY HERE, THIS IS NOT THE OPTIMAL PLACE TO CODE. ALREADY IMPLEMENTED DURING PROCCHAR INDEXING. DEBUNKED
  //  }
  Serial.println("so we have end of string " + String(end_of_string));
  if (end_of_string == (strlen(infixstring) - 1)) {
    openparenth_count != 0 ? abort() : calculate_postfix();
    Serial.println("so we didnt abort either? NICE");
  }
}

void calculate_postfix() {
  infix_key_x = 0;
  for ( int i = 1; i != 0; i = infix_stack_reference[infix_key_x] ) {
    i = infix_stack_reference[infix_key_x];   // REDUNDANCY CHECK, THIS IS REPETITIVE OF THE ACTION TO PERFORM ON EACH LOOP. ^^^^  sets it twice, make sure this line is needed or not just to simplify code.
    infix_key_x++;
    if ( i == 1 ) {
      copy(numberrepeat, 1);
      numberrepeat++;
      /* we know that the infix notation expression will always end with a 1, 
      so by piggybacking on that predictable ending, we can tie together 
      a final check to see if its actually the end of the string*/
      if ( infix_stack_reference[infix_key_x + 1] == 0 ) { // WARNING!!!!!!!!!!!!!!!!! this code might potentially be problematic if we have infix expressions that end in parenthesis.
//======== Operator Stack Popping Code
        for (int p = ((sizeof(postfix_opstack) - 1) / sizeof(int)) ; p >= 0 ; p--) {
          int opr8tr = postfix_opstack[p];
          if ( opr8tr != 0 ) { // this for loop essentially pops the stack from top to the bottom in descending order. if any alignment errors, the != 0 mediates any small calibraton issues
            copy(numberrepeat, opr8tr);
            numberrepeat++;
          }
        }
      //=====================
      }
    } // end of if i == 1 so anything below this checkes for other types of reference numbers. *ie operators or modifier characters.
    if ( i == 2 || i == 3 ) {
      pushtostack(2, i);
    } else if ( i == 4 || i == 5 ) {
      pushtostack(3, i);
    } else if ( i == 6 ) {
      pushtostack(6, i);
  }
  for ( int p = ((sizeof(postfix_stack_reference) - 1) / sizeof(int)) ; p >= 0 ; p--) {
    Serial.println("postfix_stack_reference[" + String(p) + "] =  " + String(postfix_stack_reference[p]));
  }
}

void copy(byte location, byte input ) {
  postfix_stack_reference[location] = input;
}

void pushtostack(byte precedence, int opr8tr) {
  //  Serial.println("precedence: " + String(precedence) + " operator value: " + String(opr8tr));
  //some funky stuff with checking p = 1 and it wont override it at all due to some funky stuff. Any value after p == 1 breaks the code chain and dupes the top op code for some stupid reason. go fix that bro
  for ( int p = ((sizeof(postfix_opstack) - 1) / sizeof(int)) ; p >= 0 ; p--) {
    if ( p == 0 && postfix_opstack[p] == 0 ) { //this would set the first operator into the stack considering its all 0's first and we have to make sure its the bottom one.
      postfix_opstack[p] = opr8tr;
      //      Serial.println("location is  " + String(p) + " operator value: " + String(postfix_opstack[p]));
     //        |        FIRST CONDITION    |    |  ------>  all of the rest is second branch of the OR statement -------------------->                        '                                                                       '            
    } else if ((opr8tr!=6 && precedence!=6) && (opr8tr < postfix_opstack[p]) || (opr8tr == postfix_opstack[p] || (precedence == 2 && (postfix_opstack[p] == 2 || postfix_opstack[p] == 3)) || (precedence == 3 && (postfix_opstack[p] == 4 || postfix_opstack[p] == 5))  )    )  {
      int prev_opr8tr = postfix_opstack[p];
      copy(numberrepeat, prev_opr8tr);
      numberrepeat++;
      postfix_opstack[p] = opr8tr;
      for ( int loc = p + 1 ; loc < ((sizeof(postfix_opstack) - 1) / sizeof(int)); loc ++ ) postfix_opstack[loc] = 0;
    } else if (opr8tr > postfix_opstack[p] && postfix_opstack[p] != 0)  {
      postfix_opstack[ p + 1 ] = opr8tr;
      break;
    }
  }
}

void evaluate_postfix(int p) {
  for ( int p = 0 ; p <= ((sizeof(postfix_stack_reference) - 1) / sizeof(int)) ; p++ ) {
    int number_index;
    int value = postfix_stack_reference[p];
    value == 1 ? number_index++ : number_index == number_index;
    //    value > 1 ? placeholder(DELETE THIS IT WONT COMPILE) : EXCEPTION();
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





