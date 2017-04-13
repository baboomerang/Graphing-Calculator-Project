#include <MemoryFree.h>

#include "BigNumber.h"
#include <Math.h>

char infxstr[120];          // infix string buffer from serial input
byte x = 0;                 // infix string X location

// ((3*5325)^7/3412*16)-12+(1555/12-29421)^2      test infix string for code analysis

void setup() {
  BigNumber::begin();
  BigNumber::setScale(12);
  infxstr[0] = '(';
  x = 1;
  Serial.begin(115200);
  Serial1.begin(9600);
  Serial2.begin(9600);
}

//the whole code is nested under infixdataPull();
void loop() {
  infixdataPull();
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

    char infixrawnumbersonly[100];                  // string of only the numbers together in a linear fashion. (makes it easy for cutting and sorting into the number stack)
    byte infix_stack_reference[100];                // reference key showing INFIX notation of the expression in a simplified view
    byte postfix_stack_reference[100];              // reference key showing POSTFIX notation of the expression in a simplified view
    byte postfix_opstack[25];                       // a stack used for rearranging operators to get them in PEMDAS order.
    BigNumber numberStack[25] = 0;                  // where operands are stored by index nmbrstack_FINAL[16] = "2932.231153" for example.

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

    memset(infixrawnumbersonly, 0, strlen(infixrawnumbersonly));
    memset(infix_stack_reference, 0, sizeof(infix_stack_reference));
    memset(postfix_stack_reference, 0, sizeof(postfix_stack_reference));
    memset(postfix_opstack, 0, sizeof(postfix_opstack));
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
          save_to_infix_reference(infix_stack_reference, infix_index, 6);
          break;
        case ')':
          openparenth_count -= 1;
          next_to_right_parenth = true;
          if ( cut_location != raw_index ) {
            cut_location = raw_index;
            save_num(infix_stack_reference, infix_index, infixrawnumbersonly, start, cut_location, numberStack, final_index);
          }
          save_to_infix_reference(infix_stack_reference, infix_index, 7);
          break;
        case '^':
          cut_location = raw_index;
          if (next_to_right_parenth  == false) {
            save_num(infix_stack_reference, infix_index, infixrawnumbersonly, start, cut_location, numberStack, final_index);
          } else next_to_right_parenth  = false;
          save_to_infix_reference(infix_stack_reference, infix_index, 8);
          start = cut_location;
          break;
        case '-':
          cut_location = raw_index;
          if (next_to_right_parenth  == false) {
            save_num(infix_stack_reference, infix_index, infixrawnumbersonly, start, cut_location, numberStack, final_index);
          } else next_to_right_parenth  = false;
          save_to_infix_reference(infix_stack_reference, infix_index, 2);
          start = cut_location;
          break;
        case '+':
          cut_location = raw_index;
          if (next_to_right_parenth  == false) {
            save_num(infix_stack_reference, infix_index, infixrawnumbersonly, start, cut_location, numberStack, final_index);
          } else next_to_right_parenth  = false;
          save_to_infix_reference(infix_stack_reference, infix_index, 3);
          start = cut_location;
          break;
        case '*':
          cut_location = raw_index;
          if (next_to_right_parenth  == false) {
            save_num(infix_stack_reference, infix_index, infixrawnumbersonly, start, cut_location, numberStack, final_index);
          } else next_to_right_parenth  = false;
          save_to_infix_reference(infix_stack_reference, infix_index, 4);
          start = cut_location;
          break;
        case '/':
          cut_location = raw_index;
          if (next_to_right_parenth  == false) {
            save_num(infix_stack_reference, infix_index, infixrawnumbersonly, start, cut_location, numberStack, final_index);
          } else next_to_right_parenth  = false;
          save_to_infix_reference(infix_stack_reference, infix_index, 5);
          start = cut_location;
          break;
      }
    }//end of for loop
    //============================================================================================================================================================================================================
    for (int i = 0; i < (sizeof(infix_stack_reference) / sizeof(byte)); i++) {
      Serial.print(String(infix_stack_reference[i]));
    } Serial.println("");
    return;

    //    if (openparenth_count != 0) doSomething(0); else calculate_postfix();          // '(' adds 1 to parenthcount while ')' subs 1. If parenthcount != 0 then it's mismatched parenthesis.
    //    evaluate_postfix();
  }
}

void doSomething(byte code) {
  //print out errors here, do syntax checking? who the fuck knows....
  Serial.println("we did something");
}

void save_to_infix_reference(byte infix[], byte& index, byte* value) {
  infix[index] = value;
  index++;
  /*    value range
    1 = number
    2 = subtraction
    3 = addition
    4 = multiplication                                       13 - 16 / 25 * ( 123 - 387583 ) + 691 / 9  infix expression
    5 = division                                             1  2  1 5  1 4 6  1  2    1   7 3  1  5 1   infix reference
    6 = right facing parenthesis (
    7 = left facing parenthesis )                            It's easier to manipulate this reference expression compared to the full input string
    8 = ... EXPONENT ?
    9? */
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

//            infix_stack_reference[infix_index] = 1;
//            infix_index++;
//            char buff[100];
//            String Z = String(infixrawnumbersonly);
//            String Zshort = Z.substring(start, cut_location);
//            Zshort.toCharArray(buff, Zshort.length() + 1);
//            Serial.println(buff);
//            BigNumber i = BigNumber(buff);
//            numberStack[final_index] = i;
//            final_index++;

//void save_inf_reference(byte inf_ref[], byte* i, byte* reference_type) {
//  /* reference_type value range
//    1 = number
//    2 = subtraction
//    3 = addition
//    4 = multiplication                                       13 - 16 / 25 * ( 123 - 387583 ) + 691 / 9  infix expression
//    5 = division                                             1  2  1 5  1 4 6  1  2    1   7 3  1  5 1   infix reference
//    6 = right facing parenthesis (
//    7 = left facing parenthesis )                             It's easier to manipulate this reference expression compared to the raw char string
//    8 = ... EXPONENT ?
//    9? */
//  inf_ref[i] = reference_type;
//  i++;
//}

//byte operator_process( byte& start, byte& cut, byte& x, bool& parenth, byte& final_index, byte inf[], byte* h, byte* ival) {
//  cut = x;
//  if (parenth == false) save_num(start, cut, final_index); else parenth = false;
//  save_inf_reference(inf, h, ival);
//  start = cut;
//}

/*
  Recieved infix string: (123.123*123.123
  we started savenum
  got through a decimal number
  1
  divide:  10   portion: 0.10000000000000000000
  2
  divide:  100   portion: 0.02000000000000000000
  3
  divide:  1000   portion: 0.00300000000000000000
  END OF Z
  0.12300000000000000000
  fff
  we started savenum
  got through a decimal number
  1
  divide:  10   portion: 0.10000000000000000000
  2
  divide:  100   portion: 0.02000000000000000000
  3
  divide:  1000   portion: 0.00300000000000000000
  END OF Z
  0.12300000000000000000
  fff
  Recieved infix string: (123.123*123.123)
  6141700000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
  Finished Processing, we got result approximate : 15159.27312900000000000000

  TESTED WORKING CALCULATE_DECIMAL 4/11/2017 11:12 AM
*/

//BigNumber strtbigdecimal (String& o, unsigned int& point, BigNumber& result) {
//  //Serial.println("Saving decimal part of bignum");
//  byte power = 0;
//  for (int z = point; z < (o.length() - 1); z++) {
//    power++;
//    //Serial.println(power);
//    BigNumber divide = 10;
//    divide = divide.pow(power);
//    BigNumber portion = BigNumber(int(o[z + 1]) - 48) / divide;
//    //    //Serial.print("divide:  ");
//    //    //Serial.print(divide);
//    //    //Serial.print("   portion: ");
//    //    //Serial.println(portion);
//    result += portion;
//  } //Serial.println(" END OF Z");
//  return result;
//}
//
//BigNumber strtbignum (String& o, unsigned int& point, BigNumber& result) {
//  //Serial.println("Saving integer part of bignum");
//  if (point = -1) {
//    for (int z = 0; z < (o.length()); z++) {
//      BigNumber divide = 10;
//      BigNumber portion = BigNumber(int(o[z]) - 48);
//      //      //Serial.print("divide:  ");
//      //      //Serial.print(divide);
//      //      //Serial.print("   portion: ");
//      //      //Serial.println(portion);
//      result *= 10;
//      result += portion;
//      //      //Serial.print("Result ");
//      //      //Serial.println(result);
//    } //Serial.println(" END OF Z");
//  } else {
//    for (int z = 0; z < (point - 1); z++) {
//      BigNumber divide = 10;
//      BigNumber portion = BigNumber(int(o[z]) - 48);
//      //      //Serial.print("divide:  ");
//      //      //Serial.print(divide);
//      //      //Serial.print("   portion: ");
//      //      //Serial.println(portion);
//      result *= 10;
//      result += portion;
//      //      //Serial.print("Result ");
//      //      //Serial.println(result);
//    } //Serial.println(" END OF Z");
//  }
//  return result;
//}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//void calculate_postfix(char*infix_stack_reference, byte infx_ref_x) {
//  infx_ref_x = 0;
//  for ( int i = 1; i != 0; i = infix_stack_reference[infx_ref_x] ) {
//    i = infix_stack_reference[infx_ref_x];   // REDUNDANCY CHECK, THIS IS REPETITIVE OF THE ACTION TO PERFORM ON EACH LOOP. ^^^^  sets it twice, make sure this line is needed or not just to simplify code.
//    infx_ref_x++;
//    if ( i == 1 ) {
//      copy(postfx_ref_x, 1);
//      /* we know that the infix notation expression will always end with a 1,
//        so by piggybacking on that predictable ending, we can tie together
//        a final check to see if its actually the end of the string */
//      //      if ( infix_stack_reference[infx_ref_x + 1] == 0 ) { // WARNING!!!!!!!!!!!!!!!!! this code might potentially be problematic if we have infix expressions that end in parenthesis.
//      //             ======== Operator Stack Popping Code
//      //                for (int p = ((sizeof(postfix_opstack) - 1) / sizeof(int)) ; p >= 0 ; p--) {
//      //                  int opr8tr = postfix_opstack[p];
//      //                  if ( opr8tr != 0 ) { // this for loop essentially pops the stack from top to the bottom in descending order. if any alignment errors, the != 0 mediates any small calibraton issues
//      //                    copy(postfx_ref_x, opr8tr);
//      //                  }
//      //                }
//      //              =====================
//      //      }
//    } // end of if i == 1 so anything below this checkes for other types of reference numbers. *ie operators or modifier characters.
//    if ( i == 2 || i == 3 ) {
//      pushtostack(2, i);
//    } else if ( i == 4 || i == 5 ) {
//      pushtostack(3, i);
//    } else if ( i == 6 ) {
//      pushtostack(255, i);
//    } else if ( i == 7) {
//      pushtostack(255, i);
//    } else if ( i == 8) {
//      pushtostack(4, i);
//    }
//  }
//
//  /* this prints the completed postfix_reference_stack once we finish pushing the last operator to the stack and pushtostack() exits */
//  /*for ( int p = ((sizeof(postfix_stack_reference) - 1) / sizeof(int)) ; p >= 0 ; p--) {
//    //Serial.println("postfix_stack_reference[" + String(p) + "] =  " + String(postfix_stack_reference[p]));
//    }*/
//
//}
//
//void print_opstack() {
//  delay(50);
//  //Serial.print("Opstack is : ");
//  for (int i = 0; ( i < (sizeof(postfix_opstack) / sizeof(int))); i++) {
//    //Serial.print(String(postfix_opstack[i]));
//  } //Serial.println("");
//  delay(50);
//}
//
//void copy(byte & location, byte * input ) {
//  postfix_stack_reference[location] = input;
//  postfx_ref_x++;
//}
//
//void pushtostack(byte precedence, byte opr8tr) {
//  //  print_opstack();
//  ////Serial.println("pushtostack() DEBUG: precedence: " + String(precedence) + " operator value: " + String(opr8tr));
//  //some funky stuff with checking p = 1 and it wont override it at all due to some funky stuff. Any value after p == 1 breaks the code chain and dupes the top op code for some stupid reason. go fix that bro
//  for ( byte p = ((sizeof(postfix_opstack) - 1) / sizeof(byte)) ; p >= 0 ; p--) {
//
//    if ( p == 0 && postfix_opstack[p] == 0 ) { //this would set the first operator into the stack considering its all 0's first and we have to make sure its the bottom one.
//      postfix_opstack[p] = opr8tr;
//      //      ////Serial.println("location is  " + String(p) + " operator value: " + String(postfix_opstack[p]));
//      //        |        FIRST CONDITION    |    |  ------>  all of the rest is second branch of the OR statement -------------------->                        '                                                                       '
//    } else if ((precedence != 255) && (opr8tr < postfix_opstack[p] && postfix_opstack[p] != 6) || ( (precedence == 2 && (postfix_opstack[p] == 2 || postfix_opstack[p] == 3)) || (precedence == 3 && (postfix_opstack[p] == 4 || postfix_opstack[p] == 5))  )    )  {
//      /* CODE EXPLANATION -
//        So first, we initialize a local temp variable to store the value of the fualty operator
//        then we copy that temp variable to the postfix reference stack (ie. "popping" the stack)
//        Finally, the variable in that spot "postfix_opstack[p]" is replaced with the new opr8ter given to us by the "input". (pushtostack(precedence, input oper8ter))
//        precedence is needed to generalize our input oper8tr and compare it to the actual operators already present in the opstack */
//      byte selected_oper8tr_in_opstack = postfix_opstack[p];
//      copy(postfx_ref_x, selected_oper8tr_in_opstack);
//      postfix_opstack[p] = opr8tr;
//      for ( byte loc = p + 1 ; loc < ((sizeof(postfix_opstack) - 1) / sizeof(byte)); loc ++ ) postfix_opstack[loc] = 0;
//    } else if ((opr8tr != 7 && opr8tr >= postfix_opstack[p] && postfix_opstack[p] != 0 && postfix_opstack[p] != 6))  {
//      postfix_opstack[ p + 1 ] = opr8tr;
//      break;
//    } else if ((opr8tr != 7 && opr8tr == 8 && opr8tr >= postfix_opstack[p] && postfix_opstack[p] != 0))  {
//      postfix_opstack[ p + 1 ] = opr8tr;
//      break;
//    } else if ((opr8tr != 7 && opr8tr <= postfix_opstack[p] && postfix_opstack[p] != 0 && postfix_opstack[p] == 6))  {
//      postfix_opstack[ p + 1 ] = opr8tr;
//      break;
//    } else if (precedence == 255 && opr8tr == 7) {
//      if (postfix_opstack[p] != 0) {
//        ////Serial.println("Checking the operator in the postfix_opstack[" + String(p) + "]   " + "we have : " + String(postfix_opstack[p]));
//        byte operator_2b_popped = postfix_opstack[p];
//        postfix_opstack[p] = 0;
//        if (operator_2b_popped != 6) {
//          //          delay(100);
//          ////Serial.println("=======POPPED OPERATOR========= : " + String(operator_2b_popped));
//          copy(postfx_ref_x, operator_2b_popped);
//        } else break;
//      }
//    }
//  }
//}
//
//void perform_operation(byte & input_operator, byte & pos) {
//  //  print_numberstack();
//  byte numberstack_index = -1;
//  ////Serial.println(delete_ones);
//  for (byte z = pos; z >= 0; z--) {
//    if ( postfix_stack_reference[z] == 1 ) {
//      numberstack_index++;
//      ////Serial.println("# of 1's detected when backprinting" + String(numberstack_index));
//    }
//  } numberstack_index -= delete_ones;
//  if (input_operator == 2) {
//    ////Serial.println("first operand: " + String(numberStack[numberstack_index - 1]) + " second operand: " + String(numberStack[numberstack_index]) + " SUBRACTION");
//    numberStack[numberstack_index - 1] -= numberStack[numberstack_index];
//    ////Serial.println("result" + String(numberStack[numberstack_index - 1]));
//    bring_stack_down(numberstack_index);
//    //print_numberstack();
//  }
//  if (input_operator == 3) {
//    ////Serial.println("first operand: " + String(numberStack[numberstack_index - 1]) + " second operand: " + String(numberStack[numberstack_index]) + " ADDITION");
//    numberStack[numberstack_index - 1] += numberStack[numberstack_index];
//    ////Serial.println("result" + String(numberStack[numberstack_index - 1]));
//    bring_stack_down(numberstack_index);
//    //print_numberstack();
//  }
//  //  if (input_operator == 8) numberStack[numberstack_index - 1] = numberStack[numberstack_index - 1 ] exp numberStack[numerstack_index];
//  if (input_operator == 4) {
//    ////Serial.println("first operand: " + String(numberStack[numberstack_index - 1]) + " second operand: " + String(numberStack[numberstack_index]) + " MULTIPLYING");
//    numberStack[numberstack_index - 1] *= numberStack[numberstack_index];
//    ////Serial.println("result" + String(numberStack[numberstack_index - 1]));
//    bring_stack_down(numberstack_index);
//    //print_numberstack();
//  }
//  if (input_operator == 5) {
//    ////Serial.println("first operand: " + String(numberStack[numberstack_index - 1]) + " second operand: " + String(numberStack[numberstack_index]) + " DIVIDING");
//    numberStack[numberstack_index - 1] /= numberStack[numberstack_index];
//    ////Serial.println("result" + String(numberStack[numberstack_index - 1]));
//    bring_stack_down(numberstack_index);
//    //print_numberstack();
//  }
//  if (input_operator == 8) {
//    ////Serial.println("first operand: " + String(numberStack[numberstack_index - 1]) + " second operand: " + String(numberStack[numberstack_index]) + " EXPONENTIATION");
//    numberStack[numberstack_index - 1] = numberStack[numberstack_index - 1].pow(numberStack[numberstack_index]);
//    ////Serial.println("result" + String(numberStack[numberstack_index - 1]));
//    bring_stack_down(numberstack_index);
//    //print_numberstack();
//  }
//}
//
//void bring_stack_down(byte * pop_at_this_x) {
//  for ( byte m = pop_at_this_x ; m < (sizeof(numberStack) / (sizeof(BigNumber))); m++) {
//    ////Serial.println("pop_at_this_x: " + String(m) + "  (sizeof(numberStack/sizeof(BigNumber)):  " + String((sizeof(numberStack) / (sizeof(BigNumber)))));
//    ////Serial.println("numberStack[m]: " + String(numberStack[m]) + " numberStack[m + 1]: " + String(numberStack[m + 1]));
//    if (m != (sizeof(numberStack) / (sizeof(BigNumber)))) {
//      numberStack[m] = numberStack[m + 1];
//    } else numberStack[m] = 0;
//  }
//  delete_ones++;
//}
//
////void print_numberstack() {
////  //Serial.print("Numberstack: ");
////  for ( int t = 0; t < (sizeof(numberStack) / sizeof(BigNumber)); t++) {
////    //Serial.print(String(numberStack[t]) + " ");
////  } //Serial.println(" ");
////}
//
//
//void evaluate_postfix() {
//  for ( byte p = 0 ; p <= ((sizeof(postfix_stack_reference) - 1) / sizeof(byte)) ; p++ ) {
//    byte value = postfix_stack_reference[p];
//    if (value > 1) {
//      perform_operation(value, p);
//    }
//  }
//  Serial.print("Finished Processing, we got result approximate : ");
//  Serial.print("thenumber");
//  //  Serial.println(numberStack[0]);
//  //  print_numberstack();
//}

