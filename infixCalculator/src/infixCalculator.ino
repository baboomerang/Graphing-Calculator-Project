#include <MemoryFree.h>
#include <EEPROM.h>
#include "BigNumber.h"
#include <Math.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>

#include <SPI.h>
#include <SD.h>
#include "Adafruit_HX8357.h"

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

// these are the only external variable used by the graph
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

BigNumber ox, oy;
BigNumber InputX = 0;
BigNumber InputY = 0;


char infxstr[120];          // infix string buffer from serial input
byte x = 0;                 // infix string X location

Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC);       // attach display to object declaration

/*
   Input Infix Expression and Terminal Output Results
   ((3*5325)^7/3412*16)-12+(1555/12-29421)^2

   3
   5325
   7
   3412
   16
   12                          The numbers are processed and filtered out of the string
   1555
   12
   29421
   2
   666141781514172136151217817000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
   infix reference ^  (((#*#)^#/#*#)-#+(#/#-#)^#) check the comments near 'void throwError(byte code)' declaration for instructions on interpreting the infix reference stack
   postfix_stack_reference[18] =  3
   postfix_stack_reference[17] =  8
   postfix_stack_reference[16] =  1
   postfix_stack_reference[15] =  2
   postfix_stack_reference[14] =  1
   postfix_stack_reference[13] =  5
   postfix_stack_reference[12] =  1
   postfix_stack_reference[11] =  1
   postfix_stack_reference[10] =  2
   postfix_stack_reference[9] =  1
   postfix_stack_reference[8] =  4
   postfix_stack_reference[7] =  1
   postfix_stack_reference[6] =  5
   postfix_stack_reference[5] =  1
   postfix_stack_reference[4] =  8
   postfix_stack_reference[3] =  1
   postfix_stack_reference[2] =  4
   postfix_stack_reference[1] =  1
   postfix_stack_reference[0] =  1
   Finished Processing, we got result approximate : 1245079343380762145465942456.417651751986453041552689872329
 */

/* Max Calculation Limit for an Arduino Mega 2560 is near 2^3500

   Finished Processing, we got result approximate for (2^3500):

   40270296195362184428695060755536962442278486893555705688113133546130765870172
   73715514067215023079321232763583950088951256520435312094180996588953238049534
   21455502359439932416245276659698167468088937570774479761417692998541764456595
   94188438488060010278796974460794227800534432965994490221205512053483105615556
   62969089412405585240430548127843091192984896213610464306783135661095251105384
   52853054430839857155846105630169165566758950183947324955260740763926892668470
   39632357424849669268400931224905292291149077056476503662934092443494144027797
   49666843116254069586985349675197094701615860907639669646919503637652887315356
   84692199342872936240602328625671612857527958722799557444770545725755417136296
   13597725564715311987818144011059352965537947290352570094324745683212440797155
   85249657306610450261856744620561050446305737468395539525707452118792903875893
   47246867522065584726369942916093728137773105488374703562705889962546268226061
   54512802132318476069531869703761221257941338277361836197198332730168523252328
   32105702331094682317528819996876363073536047370469376

   1054 - Total Characters in this number.

   Sketch uses 15676 bytes (6%) of program storage space. Maximum is 253952 bytes.
   Global variables use 825 bytes (10%) of dynamic memory, leaving 7367 bytes for local variables. Maximum is 8192 bytes.

   Any difference in specs will give you more or less numberspace to deal with.

   Changing the size of the char and byte arrays below will exponentially affect this upepr calculation limit.
   You have been warned.
 */

ISR( WDT_vect ) {
        EEPROM.write(2, 99);
}

void setup() {
        wdt_disable();
        watchdogSetup();

        BigNumber::begin();
        BigNumber::setScale(30);

        infxstr[0] = '(';
        x = 1;

        Serial.begin(115200);
        Serial1.begin(19200);  //Serial 1 RX pin on this MEGA should recieve information from the Attiny84 Keypad
        Serial2.begin(19200);  //Serial 2 TX pin on this MEGA should be wired to RX pin of Atmega328p
        byte Evalue = EEPROM.read(2);

        if (Evalue == 99) {
                Serial.println("OVERFLOW, RECOVERED FROM FATAL CRASH. BE CAREFUL NEXT TIME :)"); // Serial0 tells the connected computer what happened
                throwError(1); //error handler
        }

        tft.begin(HX8357D);
        tft.fillScreen(BLACK);
        tft.setRotation(1);
        //  graph_Setup(x, y, 0.01, -1, 15, 1, -10, 15, 2, "tan(abs((X-6)*(X-9)))", "X", "Y", DKBLUE, RED, LTMAGENTA, WHITE, BLACK);
}

//the whole code is nested under infixdataPull();
void loop() {
        infixdataPull();
        wdt_reset();
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

void infixdataPull() {
        if (Serial.available() > 0 || Serial1.available() > 0 || Serial2.available() > 0) { //make sure serial is open Cin
                int incomingbyte;
                if (Serial.available() > 0 ) incomingbyte = Serial.read();
                if (Serial1.available() > 0 ) incomingbyte = Serial1.read();
                if (Serial2.available() > 0 ) incomingbyte = Serial2.read();
                char input = char(incomingbyte);
                if (x <= (sizeof(infxstr)/sizeof(char)) && input != 'G' && input != 'g' && input != '=' && input != 'C' && input != 'c' && input != 'd' && input != 'D') {
                        infxstr[x] = input;
                        x++;
                        Serial.println(infxstr);
                } else if (input == '=' || input == '=') {
                        infxstr[strlen(infxstr)] = ')';                            // caps the recieved infix string with a ')'
                        process_infix_begin_calculation(infxstr, input, 0);        // once we press =, its like pressing '=' and the system starts to parse the given string.
                } else if (input == 'g' || input == 'g') {
                        infxstr[strlen(infxstr)] = ')';                            // caps the recieved infix string with a ')'
                        Graph(tft, 0, 0, 45, 290, 420, 260, -30, 30, 5, -30, 30, 5, "Sample Graph", "X", "Y", DKBLUE, RED, LTMAGENTA, WHITE, BLACK, display1);
                        byte xMIN = -30;
                        byte xMAX = 30;
                        byte yMIN = -30;
                        byte yMAX = 30;
                        BigNumber plotfreq = 10;
                        for (InputX = -30; InputX <= 30; InputX += 10) {
                                process_infix_begin_calculation(infxstr, input, 0); // once we press g, its like pressing '=' and the system starts to parse the given string.
                                Graph(tft, 0, 0, 45, 290, 420, 260, -30, 30, 5, -30, 30, 5, "Sample Graph", "X", "Y", DKBLUE, RED, LTMAGENTA, WHITE, BLACK, display1);
                                // set multiple graphing modes using that plotfreq parameter in the Graph();SS
                                // future steps though, wait for other implements.
                        }
                } else if (input == 'c' || input == 'C') {
                        memset(infxstr, 0, strlen(infxstr));                       // sets all the bytes of memory that this string takes up to 0 in the respective address
                        infxstr[0] = '(';
                        x = 1;
                        ox = 0;
                        oy = 0;
                        InputX = 0;
                        InputY = 0;                                               // these 3 lines do the same thing that void Setup() would do during initialization
                        Serial.println("Cleared String");
                        tft.fillScreen(BLACK);
                        tft.setRotation(1);
                        display1 = true;
                } else if (input == 'd' || input == 'D') {
                        infxstr[x] = '\0';
                        if(x<0) {x-=1;}
                }
        }
}
//This is where the magic begins... and ends.
void process_infix_begin_calculation(char* istr, char byteChar, byte mode) {
        if (byteChar == '=' || byteChar == 'g' || byteChar == 'G') {

                byte final_index = 0;               // saved number array position in order ie. numberStack[3] = "2323212" where final_index = 3
                byte raw_index = 0;                 // number delimiter based by quantity of integers between each operator. (goes up by 1 every # scanned by proc and set as cutting length to tokenize numbers from the RAWstack)
                byte infix_index = 0;               // infix reference stack X location
                byte postfix_index = 0;             // postfix reference X location
                //byte variable_index = 0;
                byte delete_ones = 0;               // initialize the offset to 0. everytime we perform an operation where 2 operands join into 1, we increment this number up once and later use as a negative offset to make sure we have a proper location of math done.

                char infixrawnumbersonly[150];      // string of only the numbers together in a linear fashion. (makes it easy for cutting and sorting into the number stack)
                byte infix_stack_reference[150];    // reference key showing INFIX notation of the expression in a simplified view
                byte postfix_stack_reference[150];  // reference key showing POSTFIX notation of the expression in a simplified view
                byte postfix_opstack[40];           // a stack used for rearranging operators to get them in PEMDAS order.
                BigNumber numberStack[20];          // where operands are stored by index nmbrstack_FINAL[16] = "2932.231153" for example.
                //BigNumber backupStack[20];
                //byte variable_reference[20];      // tells us which spots in the numberStack have variable X's

                /* Adjust the sizes of these stacks accordingly to your device.
                   All these calculations are done quickly and well. However, be mindful about the # of operators
                   and minimum # of operands required for proper function of the infix stack and postfix stack.
                   By the nature of this program, the Arduino UNO (Atmega328p) is physically INCAPABLE of running
                   this sketch without running out of RAM. */

                byte openparenth_count = 0;                           // count of how many open parenthesis are in the expression
                bool next_to_right_parenth = false;                   // this checks if theres a closing parenthesis ')' next to the operator. Prevents operators from saving non-existant numbers to the reference stack.
                bool operator_previously_detected = false;            // this checks if theres an operator immediately behind a right parenthesis or another operator.   '  i.e.  ++  ^/     -)   (^ '
                bool left_parenth_active = false;                     // this checks if there's an opening parenthesis '(' right before processing an operator. Prevents operators from saving non existant numbers to the reference stack.

                byte start = 0;                                       // used for saving numbers from the infixRaw array. Starting index point on the string
                byte cut_location = 0;                                // used for saving numbers from the infix raw array. Exclusion-index based cutting. so if we save a number between start of 0 and cutpoint of 4, we only save the first 3 digits.

                memset(postfix_opstack, 0, sizeof(postfix_opstack));
                memset(infixrawnumbersonly, 0, strlen(infixrawnumbersonly));
                memset(infix_stack_reference, 0, sizeof(infix_stack_reference));
                memset(postfix_stack_reference, 0, sizeof(postfix_stack_reference));
                //    memset(numberStack, 0, sizeof(numberStack));  //dont enable this line. for some reason this kills the code? gotta love memset sometimes.
                //=============================================================================================================================================================================================================
                for (byte k = 0; k < strlen(istr); k++) { // loops till the end of the string (null terminated char array)
                        char character = char(istr[k]);
                        bool error = false;
                        switch (character) {
                        case '0' ... '9':
                                operator_previously_detected = false;           //  these 2 lines make sure that any operator-parenthetical syntax checking isn't false positive.
                                left_parenth_active = false;                    //  having a number inbetween operators and parenthesis is mandatory for proper syntaax. This boolean just helps check when there is an error
                                infixrawnumbersonly[raw_index] = character;     //  infix string of 345+96-22/7-999 will create a second array 'rawnumberstack' of 34596227999
                                raw_index += 1;                                 //  index of the infixRAWnumbers array increments everytime we add a number or decimal point to it
                                break;
                        case 'X':
                                if ( operator_previously_detected == true ) {   //  implies an operator immediately behind this variable. ( 1343 + A )
                                        //save_to_reference_stack(variable_reference, variable_index, final_index);
                                        numberStack[final_index] = InputX;
                                        final_index++;
                                        // skips the savenum routine because  ^  we can predict & change what the X will be.
                                        save_to_reference_stack(infix_stack_reference, infix_index, 1); // skips the savenum (save the reference '1' to the infix reference stack).
                                        operator_previously_detected = false;   //  set the previously detected to false to prevent double variable stacking
                                } else if (operator_previously_detected == false && left_parenth_active == false) { //checks to see if there was a number or right closing parenthesis right behind.
                                        throwError(2);                          // double variable error (3+AA)
                                        return;
                                }
                                left_parenth_active = false;
                                break;
                        case '.':
                                operator_previously_detected = false;           //  these 2 lines make sure that any operator-parenthetical syntax checking isn't false positive.
                                left_parenth_active = false;                    //  having a number inbetween operators and parenthesis is mandatory for proper syntaax. This boolean just helps check when there is an error
                                infixrawnumbersonly[raw_index] = character;     //  BUGWATCHER: 4/21/2017 : there could be glitches when theres more than one decimal point in a number
                                raw_index += 1;                                 //  index of the infixRAW array increments everytime we add a number or decimal point to it
                                break;
                        case '(':
                                // if ( operator_previously_detected == false && left_parenth_active == false) {   // implies that theres a number or variable right behind the parenthesis (distributive associative property and the shorthand multiplication)
                                //         save_to_reference_stack(infix_stack_reference,infix_index,4); // 456-34(99+23)   operator_detected is set to false once encountering '34' normally we would add multiplication between that number and the next parenthesis
                                // }
                                openparenth_count += 1;
                                save_to_reference_stack(infix_stack_reference, infix_index, 6);   // check the comments below LINE 388 to know what the reference stacks do
                                left_parenth_active = true;                     // this is a left-opening parenthesis
                                break;
                        case ')':
                                left_parenth_active = false;                    // left_parenth_active is set to false any time any number is detected. this is to avoid the '( -' incorrect operator use
                                if (operator_previously_detected == true) {     // if theres an operator right behind this parenthesis without a number inbetween, its a syntax error. (ie ' (25/5+)')
                                        throwError(2);
                                        return;
                                }
                                openparenth_count -= 1;
                                next_to_right_parenth = true;                   // this is a right-closing parenthesis
                                if ( cut_location != raw_index ) {              // if the last detected operator (last updated cut_location) is different from raw index, implies a number is in between the closing parenthesis and that last operator (ie. 345+'3436') cut would be 4 but raw would be 7
                                        cut_location = raw_index;               // update the cut point to the last detcted location of a number and proceed to save the number 'inbetween' that last operator and this parenthesis
                                        save_num(infix_stack_reference, infix_index, infixrawnumbersonly, start, cut_location, numberStack, final_index);
                                }
                                save_to_reference_stack(infix_stack_reference, infix_index, 7); // check the comments below LINE 388 to know what the reference stacks do
                                break;
                        case '^':
                                error = syntax_check(operator_previously_detected,left_parenth_active); // runs some conditions regarding left parenthesis and operator status, returns true if there is an error, returns false if there is none.
                                if (error) {return;}                            // if the bool is returned as true, code EXITS process_infix_begin_calculation() on this line and we go back into infixdataPull() for input detection and reentry.
                                cut_location = raw_index;                       // the new cut location is set to the last updated raw_index position. (remember the raw_index will always be +1 after adding the last character, which is beneficial to the exclusionary limit cutting thing i talked about earlier)
                                if (next_to_right_parenth  == false) {          // without this check, having any operator next to a ' ) ' would have saved a non-existant number to the reference stack
                                        save_num(infix_stack_reference, infix_index, infixrawnumbersonly, start, cut_location, numberStack, final_index);
                                } else next_to_right_parenth = false;           // if it was true, then now its set to false because there is no longer an immediate closing parenthesis ') +'
                                save_to_reference_stack(infix_stack_reference, infix_index, 8); // check the comments below LINE 388 to know what the reference stacks do
                                start = cut_location;                           // after saving our number, the starting position to save the next number will be the last detected 'cut_location'   '1234' would be start 0 & cut 5. save the #, then update start to 5 and cut 7 ( dont forget inclusionary start and exclusionary limit behavior )
                                break;
                        case '-':
                                error = syntax_check(operator_previously_detected,left_parenth_active);
                                if (error) {return;}
                                cut_location = raw_index;
                                if (next_to_right_parenth  == false) {
                                        save_num(infix_stack_reference, infix_index, infixrawnumbersonly, start, cut_location, numberStack, final_index);
                                } else next_to_right_parenth = false;
                                save_to_reference_stack(infix_stack_reference, infix_index, 2);
                                start = cut_location;
                                break;
                        case '+':
                                error = syntax_check(operator_previously_detected,left_parenth_active);
                                if (error) {return;}
                                cut_location = raw_index;
                                if (next_to_right_parenth  == false) {
                                        save_num(infix_stack_reference, infix_index, infixrawnumbersonly, start, cut_location, numberStack, final_index);
                                } else next_to_right_parenth = false;
                                save_to_reference_stack(infix_stack_reference, infix_index, 3);
                                start = cut_location;
                                break;
                        case '*':
                                error = syntax_check(operator_previously_detected,left_parenth_active);
                                if (error) {return;}
                                cut_location = raw_index;
                                if (next_to_right_parenth  == false) {
                                        save_num(infix_stack_reference, infix_index, infixrawnumbersonly, start, cut_location, numberStack, final_index);
                                } else next_to_right_parenth = false;
                                save_to_reference_stack(infix_stack_reference, infix_index, 4);
                                start = cut_location;
                                break;
                        case '/':
                                error = syntax_check(operator_previously_detected,left_parenth_active);
                                if (error) {return;}
                                cut_location = raw_index;
                                if (next_to_right_parenth  == false) {
                                        save_num(infix_stack_reference, infix_index, infixrawnumbersonly, start, cut_location, numberStack, final_index);
                                } else next_to_right_parenth = false;
                                save_to_reference_stack(infix_stack_reference, infix_index, 5);
                                start = cut_location;
                                break;
                        }
                }//end of for loop
                 //============================================================================================================================================================================================================
                for (int i = 0; i < (sizeof(infix_stack_reference) / sizeof(byte)); i++) {
                        Serial.print(String(infix_stack_reference[i]));
                } Serial.println("");

                const int sizeofPostfixRef = ((sizeof(postfix_stack_reference) - 1) / sizeof(byte));
                const int sizeofOpstack = ((sizeof(postfix_opstack) - 1) / sizeof(byte));
                const int sizeofNumstack = ((sizeof(numberStack)) / sizeof(BigNumber));

                // '(' adds 1 to parenthcount while ')' subs 1. If parenthcount != 0 then it's because of mismatched parenthesis.
                if (openparenth_count != 0) {
                        throwError(0);
                        return;
                } else {
                        // Warning: these 2 functions have a $#!t-load of nested functions with nested arguments, worst code 2017, ill fix it at some point
                        // byte R = 20; // good to 255 elements
                        // while ( R-- ) *( backupStack + R ) = *( numberStack + R ); // dest and src are your 2 array names
                        // memcpy(backupStack,numberStack,sizeofNumstack*sizeof(BigNumber));
                        calculate_postfix(infix_stack_reference, postfix_stack_reference, postfix_opstack, postfix_index, sizeofOpstack);
                        if ( mode == 0 ) {
                                evaluate_postfix(postfix_stack_reference, numberStack, delete_ones, sizeofPostfixRef, sizeofNumstack);
                        }
                        //   else if ( mode == 2 ) {
                        // evaluate_postfix(postfix_stack_reference, numberStack, delete_ones, sizeofPostfixRef, sizeofNumstack);
                        //5 hours of my life wasted testing this shit;
                        // Serial.println("wtf bro");
                        // Graph(tft, ox, oy, 45, 290, 420, 260, -1, 15, 1, -10, 15, 2, "Sample Graph", "X", "Y", DKBLUE, RED, LTMAGENTA, WHITE, BLACK, display1);
                        // for (ox = -300; ox <= 300; ox += 1) {
                        //         byte R = 20; // good to 255 elements
                        //         while ( R-- ) *( numberStack + R ) = *( backupStack + R ); // dest and src are your 2 array names
                        //         // memcpy(numberStack, backupStack, sizeofNumstack*sizeof(BigNumber));
                        //         for(int i = 0; i<(sizeof(variable_reference)/sizeof(byte)); i++) {
                        //                 if( i != 0 && variable_reference[i] != 0 && variable_reference[i] < variable_reference[i-1] ) {
                        //                         numberStack[variable_reference[i]] = ox;
                        //                 } else if (i==0 && variable_reference[i] == 0) {
                        //                         numberStack[variable_reference[i]] = ox;
                        //                 }
                        //         }
                        //         evaluate_postfix(postfix_stack_reference, numberStack, delete_ones, sizeofPostfixRef, sizeofNumstack);
                        //         BigNumber outputY = numberStack[0];
                        //         Graph(tft, ox, outputY, 45, 290, 420, 260, -300, 300, 20, -300, 300, 20, "Sample Graph", "X", "Y", DKBLUE, RED, LTMAGENTA, WHITE, BLACK, display1);
                        // }
                        // for (inputX = Xmin; inputX <= Xmax; inputX += Plotfreq) {
                        // future steps, copy the detected x, into the reference stack, but also save a pointer or reference to it, and constantly reevaluate postfix against each update x
                        // Graph(tft, inputX, inputY, Xcorner, Ycorner, graphLength, graphHeight, Xmin, Xmax, Xinc, Ymin, Ymax, Yinc, TheTitle, Xlabels, Ylabels, gridCol, axiCol, funcCol, txtcolor, bcolor, display1);
                        // }
                }
                InputY = numberStack[0];
                memset(numberStack, 0, sizeof(numberStack));
                memset(postfix_opstack, 0, sizeof(postfix_opstack));
                memset(infixrawnumbersonly, 0, strlen(infixrawnumbersonly));
                memset(infix_stack_reference, 0, sizeof(infix_stack_reference));
                memset(postfix_stack_reference, 0, sizeof(postfix_stack_reference));
        }
}

/*  Infix Reference Key
   value range
   1 = number
   2 = subtraction
   3 = addition
   4 = multiplication                                       13 - 16 / 25 * ( 123 - 387583 ) + 691 / 9  infix expression
   5 = division                                             1  2  1 5  1 4 6  1  2    1   7 3  1  5 1   infix reference
   6 = left opening parenthesis (
   7 = right closing parenthesis )                            It's easier to manipulate this reference expression compared to the full input string
   8 = ... EXPONENT ?
   9?
 */

/*  if (cut_location==raw_index) {
      throwError(2);
      return;
    }*/
/* double operator error check:   cut_location is set to the last raw_index if they are different
   (for them to be different, it implies you processed some numbers after the last
   detected operator i.e  1236 + 123; the cut location would have last been 4 and the
   raw index now 7 )
   the only circumstance they would be the same when calling an operator is if there was another operator immediately behind it.
   THUS DOUBLE OPERATOR ERROR :P */
//

bool syntax_check (bool& operator_previously_detected, bool& left_parenth_active) {
        if (operator_previously_detected == true || left_parenth_active == true) { // if double operator, ' ++ ^/ */ -/ etc.. halt code and throw an error'
                throwError(2);
                return true;
        } else {
                operator_previously_detected = true; // if there was no operator immediately behind this one, then this currently detected one will be the new 'previously-detected' moving forward.
                return false;
        }
}

void throwError(byte code) {
        //print out errors here, do syntax checking? who the fuck knows....
        if (code == 0) {
                Serial.println("Mismatched Parenthesis");
                Serial2.write(13);
                return;
        }
        if (code == 1) {
                Serial.println("MEM-HALT Overflow");
                EEPROM.write(2, 0);
                Serial2.write(1999);
                return;
        }
        if (code == 2) {
                Serial.println("Double Operator Syntax Error");
                Serial2.write(2000);
                return;
        }
        if (code == 3) {
                Serial.println("Undefined X");
                Serial2.write(2001);
                return;
        }
}

void save_to_reference_stack(byte stack[], byte& index, byte value) {
        stack[index] = value;
        index++;
}

void save_num(byte infix_stack[], byte& infix_index, char infixRAW[], byte& start, byte& cut_location, BigNumber numberStack[], byte& final_index) {

        save_to_reference_stack(infix_stack, infix_index, 1);                   // since we are saving a number, we should add a 1 to the reference stack, indicating a #.
        // infix_stack[infix_index] = 1;
        // infix_index++;

        char buff[200];
        String Z = String(infixRAW);
        String Zshort = Z.substring(start, cut_location);
        Zshort.toCharArray(buff, Zshort.length() + 1);
        Serial.println(buff);
        BigNumber i = BigNumber(buff);

        numberStack[final_index] = i;
        final_index++;

}

void calculate_postfix(byte i_ref_stack[], byte postref[], byte opstack[], byte& pfx, const int& sizeofOpstack) {
        byte ifx = 0;
        for ( byte k = 1; k != 0; k = i_ref_stack[ifx] ) {
                k = i_ref_stack[ifx];
                ifx++;
                if ( k == 1 ) save_to_reference_stack(postref, pfx, 1);
                if ( k == 2 || k == 3 ) {
                        pushtostack(2, k, postref, opstack, pfx, sizeofOpstack);
                } else if ( k == 4 || k == 5 ) {
                        pushtostack(3, k, postref, opstack, pfx, sizeofOpstack);
                } else if ( k == 6 ) {
                        pushtostack(255, k, postref, opstack, pfx,sizeofOpstack);
                } else if ( k == 7 ) {
                        pushtostack(255, k, postref, opstack, pfx, sizeofOpstack);
                } else if ( k == 8 ) {
                        pushtostack(4, k, postref, opstack, pfx, sizeofOpstack);
                }
        }
        /* this prints the completed postfix_reference_stack once we finish pushing the last operator to the stack and pushtostack() exits */
        for ( int p = sizeofOpstack; p >= 0; p--) {
                Serial.println("postfix_stack_reference[" + String(p) + "] =  " + String(postref[p]));
        }
}

void pushtostack(byte precedence, byte opr8tr, byte post[], byte postfix_opstack[], byte& pfx, const int& sizeofOpstack) {
        //Serial.println("pushtostack() DEBUG: precedence: " + String(precedence) + " operator value: " + String(opr8tr));
        for ( int p = sizeofOpstack; p >= 0; p--) {
                if ( p == 0 && postfix_opstack[p] == 0 ) { //this would set the first operator into the stack considering its all 0's first and we have to make sure its the bottom one.
                        postfix_opstack[p] = opr8tr;
                        //      //Serial.println("location is  " + String(p) + " operator value: " + String(postfix_opstack[p]));
                } else if ((precedence != 255) && (opr8tr < postfix_opstack[p] && postfix_opstack[p] != 6) || ( (precedence == 2 && (postfix_opstack[p] == 2 || postfix_opstack[p] == 3)) || (precedence == 3 && (postfix_opstack[p] == 4 || postfix_opstack[p] == 5))  )    )  {
                        /* CODE EXPLANATION -
                           So first, we initialize a local temp variable to store the value of the fualty operator
                           then we copy that temp variable to the postfix reference stack (ie. "popping" the stack)
                           Finally, the variable in that spot "postfix_opstack[p]" is replaced with the new opr8ter given to us by the "input". (pushtostack(precedence, input oper8ter))
                           precedence is needed to generalize our input oper8tr and compare it to the actual operators already present in the opstack */
                        int selected_oper8tr_in_opstack = postfix_opstack[p];
                        save_to_reference_stack(post, pfx, selected_oper8tr_in_opstack);
                        postfix_opstack[p] = opr8tr;
                        for ( int loc = p + 1; loc < sizeofOpstack; loc++ ) postfix_opstack[loc] = 0;
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
                                        //Serial.println("=======POPPED OPERATOR========= : " + String(operator_2b_popped));
                                        save_to_reference_stack(post, pfx, operator_2b_popped);
                                } else break;
                        }
                }
        }
}


void evaluate_postfix(byte post[], BigNumber num_Stack[], byte& offset,  const int& sizeofPostfixRef, const int& sizeofNumstack) {
        for ( int p = 0; p <= sizeofPostfixRef; p++ ) {
                byte value = post[p];
                if (value > 1) {
                        perform_operation(value, p, post, num_Stack, offset, sizeofNumstack);
                }
        }
        Serial.print("Finished Processing, we got result approximate : ");
        Serial.println(num_Stack[0]);
}

void perform_operation(byte & input_operator, int & pos, byte post[], BigNumber num_Stack[], byte& offset, const int& sizeofNumstack) {
        byte index = -1;
        for (int z = pos; z >= 0; z--) {
                if ( post[z] == 1 ) index++;
        }
        index -= offset;
        if (input_operator == 2) {
                num_Stack[index - 1] -= num_Stack[index];
                bring_stack_down(index, num_Stack, sizeofNumstack);
                offset++;
        }
        if (input_operator == 3) {
                num_Stack[index - 1] += num_Stack[index];
                bring_stack_down(index, num_Stack, sizeofNumstack);
                offset++;
        }
        if (input_operator == 4) {
                num_Stack[index - 1] *= num_Stack[index];
                bring_stack_down(index, num_Stack, sizeofNumstack);
                offset++;
        }
        if (input_operator == 5) {
                num_Stack[index - 1] /= num_Stack[index];
                bring_stack_down(index, num_Stack, sizeofNumstack);
                offset++;
        }
        if (input_operator == 8) {
                num_Stack[index - 1] = num_Stack[index - 1].pow(num_Stack[index]);
                bring_stack_down(index, num_Stack, sizeofNumstack);
                offset++;
        }
}

void bring_stack_down(byte& bring_to_this_x, BigNumber num_Stack[], const int& sizeofNumstack) {
        for ( int m = bring_to_this_x; m < sizeofNumstack; m++) {
                if (m != sizeofNumstack) {
                        num_Stack[m] = num_Stack[m + 1];
                } else num_Stack[m] = 0;
        }
}
//===================================================================================================================================================================================
void graph_Setup(double inputX, double inputY, double Plotfreq, double Xmin, double Xmax, double Xinc, double Ymin, double Ymax, double Yinc, String TheTitle, String Xlabels, String Ylabels, unsigned int gridCol, unsigned int axiCol, unsigned int funcCol, unsigned int txtcolor, unsigned int bcolor ) {
        int Xcorner = 45;
        int Ycorner = 290;
        for (inputX = Xmin; inputX <= Xmax; inputX += Plotfreq) {
                //    inputY = inputX * inputX + 6 * inputX - 9;
                inputY = tan(abs((inputX - 6) * (inputX - 9)));
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
                d.setCursor(gx, gy - h - 30);
                d.println(title);

                d.setTextSize(1);
                d.setTextColor(acolor, bcolor);
                d.setCursor(gx, gy + 20);
                d.println(xlabel);

                d.setTextSize(1);
                d.setTextColor(acolor, bcolor);
                d.setCursor(gx - 30, gy - h - 10);
                d.println(ylabel);


        }

        //graph drawn now plot the data
        // the entire plotting code are these few lines...
        // recall that ox and oy are initialized as static above
        BigNumber one = '1';
        x =  (x - xlo) * ( w) / (xhi - xlo) + gx;
        y =  (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
        d.drawLine(ox, oy, x, y, pcolor);
        d.drawLine(ox, oy + one, x, y + one, pcolor);
        d.drawLine(ox, oy - one, x, y - one, pcolor);
        ox = x;
        oy = y;

}
