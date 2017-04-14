#include <MemoryFree.h>
#include <EEPROM.h>
#include "BigNumber.h"
#include <Math.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>
char infxstr[120];          // infix string buffer from serial input
byte x = 0;                 // infix string X location

// ((3*5325)^7/3412*16)-12+(1555/12-29421)^2      test infix string for code analysis

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
  Serial1.begin(19200);
  Serial2.begin(19200);
  byte Evalue = EEPROM.read(2);
  if (Evalue == 99) {
    EEPROM.write(2, 0);
    Serial.println("OVERFLOW, RECOVERED FROM FATAL CRASH. BE CAREFUL NEXT TIME :)");
  }
}

//the whole code is nested under infixdataPull();
void loop() {
  infixdataPull();
  wdt_reset();
}

void watchdogSetup(void) {
  cli();  // disable all interrupts
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
    if (input != 'G' && input != 'g' && input != '=' && input != 'C' && input != 'c') {
      infxstr[x] = input;
      x++;
    } else if (input == 'g' || input == 'G') {
      infixproc(infxstr, input);
    } else if (input == 'c' || input == 'C') {
      memset(infxstr, 0, strlen(infxstr));
      infxstr[0] = '(';
      x = 1;
      Serial.println("Cleared String");
    }
  }
}

void infixproc(char* istr, char* byteChar) {  // INFIX PROC DOES EXACTLY WHAT GETLINE DOES IN C++, but arduino for some absurd reason, does not have C++ STDL so I had to manually code it in
  if (byteChar == 'g' || byteChar == 'G') {

    byte final_index = 0;                           // saved number array position in order ie. numberStack[3] = "2323212" where final_index = 3
    byte raw_index = 0;                             // number delimiter based by quantity of integers between each operator. (goes up by 1 every # scanned by proc and set as cutting length to tokenize numbers from the RAWstack)
    byte infix_index = 0;                           // infix reference stack X location
    byte postfix_index = 0;                         // postfix reference X location
    byte delete_ones = 0;                           // initialize the offset to 0. everytime we perform an operation where 2 operands join into 1, we increment this number up once and later use as a negative offset to make sure we have a proper location of math done.

    char infixrawnumbersonly[150];                  // string of only the numbers together in a linear fashion. (makes it easy for cutting and sorting into the number stack)
    byte infix_stack_reference[150];                // reference key showing INFIX notation of the expression in a simplified view
    byte postfix_stack_reference[150];              // reference key showing POSTFIX notation of the expression in a simplified view
    byte postfix_opstack[40];                       // a stack used for rearranging operators to get them in PEMDAS order.
    BigNumber numberStack[20] = 0;                  // where operands are stored by index nmbrstack_FINAL[16] = "2932.231153" for example.

    /* Adjust the sizes of these stacks accordingly to your device.
      All these calculations are done quickly and well. However, be mindful about the # of operators
      and minimum # of operands required for proper function of the infix stack and postfix stack.
      By the nature of this program, the Arduino UNO (Atmega328p) is physically INCAPABLE of running
      this sketch without running out of RAM. */

    byte openparenth_count = 0;                            //count of how many open parenthesis are in the expression
    bool next_to_right_parenth = false;                    //this checks if theres a closing parenthesis ')' next to the operator. Prevents operators from saving non-existant numbers to the reference stack.

    byte start = 0;
    byte cut_location = 0;

    istr[strlen(istr)] = ')';                              // caps the recieved infix string with a ')'

    memset(postfix_opstack, 0, sizeof(postfix_opstack));
    memset(infixrawnumbersonly, 0, strlen(infixrawnumbersonly));
    memset(infix_stack_reference, 0, sizeof(infix_stack_reference));
    memset(postfix_stack_reference, 0, sizeof(postfix_stack_reference));
    //    memset(numberStack, 0, sizeof(numberStack));  //dont enable this line. for some reason this kills the code? gotta love memset sometimes.
    //=============================================================================================================================================================================================================
    for (byte k = 0; k < strlen(istr); k++) {
      char character = char(istr[k]);
      switch (character) {
        case '0' ... '9':
          infixrawnumbersonly[raw_index] = character;                   //  infix string of 345+96-22/7-999 will become rawnumberstack of 34596227999
          raw_index += 1;
          break;
        case '.':
          infixrawnumbersonly[raw_index] = character;
          raw_index += 1;
          break;
        case '(':
          openparenth_count += 1;
          save_to_reference_stack(infix_stack_reference, infix_index, 6);
          break;
        case ')':
          openparenth_count -= 1;
          next_to_right_parenth = true;
          if ( cut_location != raw_index ) {
            cut_location = raw_index;
            save_num(infix_stack_reference, infix_index, infixrawnumbersonly, start, cut_location, numberStack, final_index);
          }
          save_to_reference_stack(infix_stack_reference, infix_index, 7);
          break;
        case '^':
          cut_location = raw_index;
          if (next_to_right_parenth  == false) {
            save_num(infix_stack_reference, infix_index, infixrawnumbersonly, start, cut_location, numberStack, final_index);
          } else next_to_right_parenth  = false;
          save_to_reference_stack(infix_stack_reference, infix_index, 8);
          start = cut_location;
          break;
        case '-':
          cut_location = raw_index;
          if (next_to_right_parenth  == false) {
            save_num(infix_stack_reference, infix_index, infixrawnumbersonly, start, cut_location, numberStack, final_index);
          } else next_to_right_parenth  = false;
          save_to_reference_stack(infix_stack_reference, infix_index, 2);
          start = cut_location;
          break;
        case '+':
          cut_location = raw_index;
          if (next_to_right_parenth  == false) {
            save_num(infix_stack_reference, infix_index, infixrawnumbersonly, start, cut_location, numberStack, final_index);
          } else next_to_right_parenth  = false;
          save_to_reference_stack(infix_stack_reference, infix_index, 3);
          start = cut_location;
          break;
        case '*':
          cut_location = raw_index;
          if (next_to_right_parenth  == false) {
            save_num(infix_stack_reference, infix_index, infixrawnumbersonly, start, cut_location, numberStack, final_index);
          } else next_to_right_parenth  = false;
          save_to_reference_stack(infix_stack_reference, infix_index, 4);
          start = cut_location;
          break;
        case '/':
          cut_location = raw_index;
          if (next_to_right_parenth  == false) {
            save_num(infix_stack_reference, infix_index, infixrawnumbersonly, start, cut_location, numberStack, final_index);
          } else next_to_right_parenth  = false;
          save_to_reference_stack(infix_stack_reference, infix_index, 5);
          start = cut_location;
          break;
      }
    }//end of for loop
    //============================================================================================================================================================================================================
    for (int i = 0; i < (sizeof(infix_stack_reference) / sizeof(byte)); i++) {
      Serial.print(String(infix_stack_reference[i]));
    } Serial.println("");
    // '(' adds 1 to parenthcount while ')' subs 1. If parenthcount != 0 then it's because of mismatched parenthesis.
    int a = ((sizeof(postfix_stack_reference) - 1) / sizeof(byte));
    int b = ((sizeof(postfix_opstack) - 1) / sizeof(byte));
    int c = ((sizeof(numberStack)) / sizeof(BigNumber));
    if (openparenth_count != 0) doSomething(0); else calculate_postfix( infix_stack_reference, postfix_stack_reference, postfix_opstack, postfix_index, a, b, c );
    evaluate_postfix(postfix_stack_reference, numberStack, delete_ones, a , c);
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
  6 = right facing parenthesis (
  7 = left facing parenthesis )                            It's easier to manipulate this reference expression compared to the full input string
  8 = ... EXPONENT ?
  9?
*/


void doSomething(byte code) {
  //print out errors here, do syntax checking? who the fuck knows....
  Serial.println("we did something");
}

void save_to_reference_stack(byte infix[], byte& index, byte value) {
  infix[index] = value;
  index++;
}

void save_num(byte infix_stack[], byte& index, char infixRAW[], byte& start, byte& cut_location, BigNumber numberStack[], byte& d) {
  infix_stack[index] = 1;
  index++;
  char buff[100];
  String Z = String(infixRAW);
  String Zshort = Z.substring(start, cut_location);
  Zshort.toCharArray(buff, Zshort.length() + 1);
  Serial.println(buff);
  BigNumber i = BigNumber(buff);
  numberStack[d] = i;
  d++;
}

void calculate_postfix(byte i_ref_stack[], byte post[], byte opstack[], byte& pfx, int& a, int& b, int& c) {
  byte ifx = 0;
  for ( byte k = 1; k != 0; k = i_ref_stack[ifx] ) {
    k = i_ref_stack[ifx];
    ifx++;
    if ( k == 1 ) save_to_reference_stack(post, pfx, 1);
    if ( k == 2 || k == 3 ) {
      pushtostack(2, k, post, opstack, pfx, a, b, c);
    } else if ( k == 4 || k == 5 ) {
      pushtostack(3, k, post, opstack, pfx, a, b, c);
    } else if ( k == 6 ) {
      pushtostack(255, k, post, opstack, pfx, a, b, c);
    } else if ( k == 7 ) {
      pushtostack(255, k, post, opstack, pfx, a, b, c);
    } else if ( k == 8 ) {
      pushtostack(4, k, post, opstack, pfx, a, b, c);
    }
  }
  /* this prints the completed postfix_reference_stack once we finish pushing the last operator to the stack and pushtostack() exits */
  for ( int p = b; p >= 0 ; p--) {
    Serial.println("postfix_stack_reference[" + String(p) + "] =  " + String(post[p]));
  }
}

void pushtostack(byte precedence, byte opr8tr, byte post[], byte postfix_opstack[], byte& pfx, int& a, int& b, int& c) {
  //  print_opstack();
  //Serial.println("pushtostack() DEBUG: precedence: " + String(precedence) + " operator value: " + String(opr8tr));
  //some funky stuff with checking p = 1 and it wont override it at all due to some funky stuff. Any value after p == 1 breaks the code chain and dupes the top op code for some stupid reason. go fix that bro
  for ( int p = b ; p >= 0 ; p--) {

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
      save_to_reference_stack(post, pfx, selected_oper8tr_in_opstack);
      //      copy(numberrepeat, selected_oper8tr_in_opstack);
      postfix_opstack[p] = opr8tr;
      for ( int loc = p + 1 ; loc < b; loc ++ ) postfix_opstack[loc] = 0;
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
          save_to_reference_stack(post, pfx, operator_2b_popped);
        } else break;
      }
    }
  }
}


void evaluate_postfix(byte post[], BigNumber num_Stack[], byte& offset, int& a, int& c) {
  for ( int p = 0 ; p <= a ; p++ ) {
    byte value = post[p];
    if (value > 1) {
      perform_operation(value, p, post, num_Stack, offset, c);
    }
  }
  Serial.print("Finished Processing, we got result approximate : ");
  Serial.println(num_Stack[0]);
}

void perform_operation(byte & input_operator, int & pos, byte post[], BigNumber num_Stack[], byte& offset, int& c) {
  byte index = -1;
  for (int z = pos; z >= 0; z--) {
    if ( post[z] == 1 ) index++;
  }
  index -= offset;
  if (input_operator == 2) {
    num_Stack[index - 1] -= num_Stack[index];
    bring_stack_down(index, num_Stack, offset, c);
  }
  if (input_operator == 3) {
    num_Stack[index - 1] += num_Stack[index];
    bring_stack_down(index, num_Stack, offset, c);
  }
  if (input_operator == 4) {
    num_Stack[index - 1] *= num_Stack[index];
    bring_stack_down(index, num_Stack, offset, c);
  }
  if (input_operator == 5) {
    num_Stack[index - 1] /= num_Stack[index];
    bring_stack_down(index, num_Stack, offset, c);
  }
  if (input_operator == 8) {
    num_Stack[index - 1] = num_Stack[index - 1].pow(num_Stack[index]);
    bring_stack_down(index, num_Stack, offset, c);
  }
}

void bring_stack_down(byte& pop_at_this_x, BigNumber num_Stack[], byte& offset, int& c) {
  for ( int m = pop_at_this_x ; m < c; m++) {
    if (m != c) {
      num_Stack[m] = num_Stack[m + 1];
    } else num_Stack[m] = 0;
  }
  offset++;
}