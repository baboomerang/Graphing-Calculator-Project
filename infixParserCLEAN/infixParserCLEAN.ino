#include <MemoryFree.h>

#include "BigNumber.h"
#include <Math.h>

//input / cin variables
char byteChar;

//Cin input X location of infix expression (infX) printing from left to right
byte infX = 0;
// saved number array position in order ie. numberStack_FINAL[3] = "2323212" where num_indx = 3
byte num_indx = 0;
// number delimiter based by operator dependency
byte num_x = 0;
//infix reference stack X location
byte infix_key_x = 0;
//postfix reference X location
byte numberrepeat = 0;

// initialize the offset to 0. everytime we perform an operation where 2 operands join into 1, we increment this number up once to make sure we have a proper location of math done.
byte delete_ones = 0;

//infixnumberstack and the Cin String
char infixRAWnumberStack[100];  //string of only the numbers together in a linear fashion. (makes it easy for cutting and sorting into the number stack)
char infixstring[100]; // infix string buffer from serial input

byte infix_stack_reference[100]; // reference key showing INFIX notation of the expression in a simplified view
byte postfix_stack_reference[100]; // reference key showing POSTFIX notation of the expression in a simplified view

byte postfix_opstack[25]; // a stack used for rearranging operators to get them in PEMDAS order.
BigNumber numberStack_FINAL[25] = 0; // where operands are stored by index nmbrstack_FINAL[16] = "2932.231153" for example.

/* adjust the sizes of these stacks accordingly to your device,
  all these calculations are done rather quickly and well, but be mindful of the amount of operators
  and minimum # of operands required for proper function of the infix stack and postfix stack.*/

//count of how many open parenthesis are in the expression
unsigned int openparenth_count = 0;
//this is an immediate operator to parenthesis checker. Prevents operators from performing arbitrary operations on non-existing operands.
bool active_parenth = false;


// ((3*5325)^7/3412*16)-12+(1555/12-29421)^2      test infix string for code analysis

void setup() {
  BigNumber::begin();        //                                                                                                          THIS IS WHERE THE BIGNUMBER LIBRARY BEGINS
  BigNumber::setScale(12);
  infixstring[0] = '(';
  infX = 1;
  Serial.begin(115200);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial.print("freeMemory()=");
  Serial.println(freeMemory());
}

//the whole code is nested under infixdataPull();
void loop() {
  infixdataPull();
}

void doNothing(byte code) {
  if (code == 0) active_parenth = false;
  //Serial.println("we are doing NOTHING");
}

void infixdataPull() {
  if (Serial.available() > 0 || Serial1.available() > 0 || Serial2.available() > 0) { //make sure serial is open Cin
    int incomingbyte;
    if (Serial.available() > 0 ) incomingbyte = Serial.read();
    if (Serial1.available() > 0 ) incomingbyte = Serial1.read();
    if (Serial2.available() > 0 ) incomingbyte = Serial2.read();
    byteChar = char(incomingbyte);
    (byteChar != 'g' && byteChar != '=' && byteChar != 'C' && byteChar != 'c') ? infixstring[infX] = byteChar : infixstring[infX - 1] = infixstring[infX - 1];
    infX += 1;
    Serial.println("Recieved infix string: " + String(infixstring));
    //=============== INFIXPARSER ========
    infixproc();                       // it will loop infixproc infinitely. infixproc will continue with its own subroutines once the character "g" is detected.
    //====================================
  }
}

void infixproc() {  // INFIX PROC DOES EXACTLY WHAT GETLINE DOES IN C++, but arduino for some absurd reason, does not have C++ STDL so I had to manually code it in
  if (byteChar == 'c' || byteChar == 'C') {
    //Serial.print("freeMemory()=");
    //Serial.println(freeMemory());
    memset(infix_stack_reference, 0, sizeof(infix_stack_reference));
    memset(postfix_stack_reference, 0, sizeof(postfix_stack_reference));
    memset(postfix_opstack, 0, sizeof(postfix_opstack));
    //    memset(numberStack_FINAL, 0, sizeof(numberStack_FINAL));
    memset(infixstring, 0, strlen(infixstring));
    memset(infixRAWnumberStack, 0, strlen(infixRAWnumberStack));

    for (byte i = 0; i < (sizeof(infix_stack_reference) / sizeof(byte)); i++) {
      infix_stack_reference[i] = 0;
      postfix_stack_reference[i] = 0;
    }

    for (byte i = 0; i < (sizeof(postfix_opstack) / sizeof(byte)); i++) {
      postfix_opstack[i] = 0;
    }

    //    for (int i = 0; i <= (strlen(infixstring)); i++) {
    //      infixstring[i] = ' ';
    //    }
    //    for (int i = 0; i < (strlen(infixRAWnumberStack)); i++) {
    //      infixRAWnumberStack[i] = '0';
    //    }

    for (byte i = 0; i < (sizeof(numberStack_FINAL) / sizeof(BigNumber)); i++) {
      numberStack_FINAL[i] = 0;
    }

    infixstring[0] = '(';
    infX = 1;
    numberrepeat = 0;
    infix_key_x = 0;
    num_x = 0;
    num_indx = 0;
    delete_ones = 0;
    openparenth_count = 0;
    active_parenth = false;

    Serial.println("CLEARED infix string: " + String(infixstring));
  }
  if (byteChar == 'g') {
    //    byteChar = ' ';
    byte start = 0;
    byte cutHere = 0;
    //Serial.println("Step 2");
    infixstring[strlen(infixstring)] = ')';
    //once we press g, it will start to process the string entirely. thanks for the tip angad!
    for (byte i = 0; i < strlen(infixstring); i++) { //from the starting point of the infix string until the end of the string.....
      char procChar = char(infixstring[i]);
      //Serial.println("Step 3");
      //Serial.print("freeMemory()=");
      //Serial.println(freeMemory());
      switch (procChar) {
        case '0' ... '9':
          infixRAWnumberStack[num_x] = procChar;  //  infix string of 345+96-22/7-999 will become rawnumberstack of 34596227999 and depending on the distance between the sequence of numbers to the nearest operator, we can delimit and "cut" each number out of this raw stack like a stencil.
          num_x += 1;
          break;
        case '.':
          infixRAWnumberStack[num_x] = procChar;
          num_x += 1;
          break;
        case '(':
          openparenth_count += 1;
          save_op(6);
          break;
        case ')':
          openparenth_count -= 1;
          active_parenth = true;
          if ( cutHere != num_x ) {
            cutHere = num_x;
            save_num(start, cutHere, num_indx);
          }
          save_op(7); // this line IS VERY ORDER SENSITIVE, DO NOT CHANGE THE ORDER OTHERWISE STUFF BREAKS. THIS LINE HAS TO BE HERE
          if (openparenth_count == 0) {
            ////Serial.println(F("PARENTH LOGIC DEBUG: PARENTH COUNT IS 0"));
            END_OF_STRING_START_EVALUATING(i); //=================================================== as a terrible design choice, this whole program continues by calling the syntax check soubroutine
          }
          break;
        case '^':
          cutHere = num_x;
          active_parenth == false ? save_num(start, cutHere, num_indx) : doNothing(0);
          save_op(8);
          start = cutHere;
          break;
        case '-':
          cutHere = num_x;
          active_parenth == false ? save_num(start, cutHere, num_indx) : doNothing(0);
          save_op(2);
          start = cutHere;
          break;
        case '+':
          cutHere = num_x;
          active_parenth == false ? save_num(start, cutHere, num_indx) : doNothing(0);
          save_op(3);
          start = cutHere;
          break;
        case '*':
          cutHere = num_x;
          active_parenth == false ? save_num(start, cutHere, num_indx) : doNothing(0);
          save_op(4);
          start = cutHere;
          break;
        case '/':
          cutHere = num_x;
          active_parenth == false ? save_num(start, cutHere, num_indx) : doNothing(0);
          save_op(5);
          start = cutHere;
          break;
      }
    }
    //Serial.println("Recieved infix string: " + String(infixstring));
    //Serial.print(" RIGHT BEFORE EVALUATING POSTFIX freeMemory()=");
    //Serial.println(freeMemory());

    //    print_infix();
    evaluate_postfix();
  }

}

//void print_infix() {
//  for (int i = 0; i < (sizeof(infix_stack_reference) / sizeof(int)); i++) {
//    //Serial.print(String(infix_stack_reference[i]));
//  } //Serial.println("");
//}

void save_num(byte& start, byte& cutpoint, byte& index_xpos) {

  char buff[100];
  save_op(1);

  String Z = String(infixRAWnumberStack);
  String Zshort = Z.substring(start, cutpoint);

  Zshort.toCharArray(buff, Zshort.length() + 1);
  Serial.println(buff);
  BigNumber i = BigNumber(buff);
  //  BigNumber i;
  //  if (dec_point != -1) {
  //    unsigned int scnd_dec_point = Zshort.indexOf('.', dec_point + 1);
  //    if (scnd_dec_point == -1) {
  //      //Serial.println("got through a decimal number");
  //      BigNumber bigdecimal = 0;
  //      strtbigdecimal(Zshort, dec_point, bigdecimal);
  //      strtbignum(Zshort, dec_point, i);
  //      i += BigNumber(bigdecimal);
  //      //      free(bigdecimal);
  //    } else Serial.print("SYNTAX ERROR, EXTRA DECIMAL");
  //  } else if (dec_point == -1) strtbignum(Zshort, dec_point, i);
  numberStack_FINAL[index_xpos] = i;
  num_indx++;
}

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

void save_op(byte* reference_type) {
  /* reference_type value range
    1 = number
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

void END_OF_STRING_START_EVALUATING(byte& end_of_string) {
  //checks every closing parenthesis if its the end of the string. If it is the final parenthesis at the end of the string, check for unbalanced parenthesis.
  // this parenthesis (  adds 1 to the openparenth count, while ) subtracts 1 from the open parenth count. if at the end of the string this variable isnt 0, then mismatched parenthesis.
  if (end_of_string == (strlen(infixstring) - 1)) {
    openparenth_count != 0 ? doNothing(0) : calculate_postfix();
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
    //Serial.println("postfix_stack_reference[" + String(p) + "] =  " + String(postfix_stack_reference[p]));
    }*/

}

void print_opstack() {
  delay(50);
  //Serial.print("Opstack is : ");
  for (int i = 0; ( i < (sizeof(postfix_opstack) / sizeof(int))); i++) {
    //Serial.print(String(postfix_opstack[i]));
  } //Serial.println("");
  delay(50);
}

void copy(byte& location, byte* input ) {
  postfix_stack_reference[location] = input;
  numberrepeat++;
}

void pushtostack(byte precedence, byte opr8tr) {
  //  print_opstack();
  ////Serial.println("pushtostack() DEBUG: precedence: " + String(precedence) + " operator value: " + String(opr8tr));
  //some funky stuff with checking p = 1 and it wont override it at all due to some funky stuff. Any value after p == 1 breaks the code chain and dupes the top op code for some stupid reason. go fix that bro
  for ( byte p = ((sizeof(postfix_opstack) - 1) / sizeof(byte)) ; p >= 0 ; p--) {

    if ( p == 0 && postfix_opstack[p] == 0 ) { //this would set the first operator into the stack considering its all 0's first and we have to make sure its the bottom one.
      postfix_opstack[p] = opr8tr;
      //      ////Serial.println("location is  " + String(p) + " operator value: " + String(postfix_opstack[p]));
      //        |        FIRST CONDITION    |    |  ------>  all of the rest is second branch of the OR statement -------------------->                        '                                                                       '
    } else if ((precedence != 255) && (opr8tr < postfix_opstack[p] && postfix_opstack[p] != 6) || ( (precedence == 2 && (postfix_opstack[p] == 2 || postfix_opstack[p] == 3)) || (precedence == 3 && (postfix_opstack[p] == 4 || postfix_opstack[p] == 5))  )    )  {
      /* CODE EXPLANATION -
        So first, we initialize a local temp variable to store the value of the fualty operator
        then we copy that temp variable to the postfix reference stack (ie. "popping" the stack)
        Finally, the variable in that spot "postfix_opstack[p]" is replaced with the new opr8ter given to us by the "input". (pushtostack(precedence, input oper8ter))
        precedence is needed to generalize our input oper8tr and compare it to the actual operators already present in the opstack */
      byte selected_oper8tr_in_opstack = postfix_opstack[p];
      copy(numberrepeat, selected_oper8tr_in_opstack);
      postfix_opstack[p] = opr8tr;
      for ( byte loc = p + 1 ; loc < ((sizeof(postfix_opstack) - 1) / sizeof(byte)); loc ++ ) postfix_opstack[loc] = 0;
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
        ////Serial.println("Checking the operator in the postfix_opstack[" + String(p) + "]   " + "we have : " + String(postfix_opstack[p]));
        byte operator_2b_popped = postfix_opstack[p];
        postfix_opstack[p] = 0;
        if (operator_2b_popped != 6) {
          //          delay(100);
          ////Serial.println("=======POPPED OPERATOR========= : " + String(operator_2b_popped));
          copy(numberrepeat, operator_2b_popped);
        } else break;
      }
    }
  }
}

void perform_operation(byte& input_operator, byte& pos) {
  //  print_numberstack();
  byte numberstack_index = -1;
  ////Serial.println(delete_ones);
  for (byte z = pos; z >= 0; z--) {
    if ( postfix_stack_reference[z] == 1 ) {
      numberstack_index++;
      ////Serial.println("# of 1's detected when backprinting" + String(numberstack_index));
    }
  } numberstack_index -= delete_ones;
  if (input_operator == 2) {
    ////Serial.println("first operand: " + String(numberStack_FINAL[numberstack_index - 1]) + " second operand: " + String(numberStack_FINAL[numberstack_index]) + " SUBRACTION");
    numberStack_FINAL[numberstack_index - 1] -= numberStack_FINAL[numberstack_index];
    ////Serial.println("result" + String(numberStack_FINAL[numberstack_index - 1]));
    bring_stack_down(numberstack_index);
    //print_numberstack();
  }
  if (input_operator == 3) {
    ////Serial.println("first operand: " + String(numberStack_FINAL[numberstack_index - 1]) + " second operand: " + String(numberStack_FINAL[numberstack_index]) + " ADDITION");
    numberStack_FINAL[numberstack_index - 1] += numberStack_FINAL[numberstack_index];
    ////Serial.println("result" + String(numberStack_FINAL[numberstack_index - 1]));
    bring_stack_down(numberstack_index);
    //print_numberstack();
  }
  //  if (input_operator == 8) numberstack_FINAL[numberstack_index - 1] = numberstack_FINAL[numberstack_index - 1 ] exp numberstack_FINAL[numerstack_index];
  if (input_operator == 4) {
    ////Serial.println("first operand: " + String(numberStack_FINAL[numberstack_index - 1]) + " second operand: " + String(numberStack_FINAL[numberstack_index]) + " MULTIPLYING");
    numberStack_FINAL[numberstack_index - 1] *= numberStack_FINAL[numberstack_index];
    ////Serial.println("result" + String(numberStack_FINAL[numberstack_index - 1]));
    bring_stack_down(numberstack_index);
    //print_numberstack();
  }
  if (input_operator == 5) {
    ////Serial.println("first operand: " + String(numberStack_FINAL[numberstack_index - 1]) + " second operand: " + String(numberStack_FINAL[numberstack_index]) + " DIVIDING");
    numberStack_FINAL[numberstack_index - 1] /= numberStack_FINAL[numberstack_index];
    ////Serial.println("result" + String(numberStack_FINAL[numberstack_index - 1]));
    bring_stack_down(numberstack_index);
    //print_numberstack();
  }
  if (input_operator == 8) {
    ////Serial.println("first operand: " + String(numberStack_FINAL[numberstack_index - 1]) + " second operand: " + String(numberStack_FINAL[numberstack_index]) + " EXPONENTIATION");
    numberStack_FINAL[numberstack_index - 1] = numberStack_FINAL[numberstack_index - 1].pow(numberStack_FINAL[numberstack_index]);
    ////Serial.println("result" + String(numberStack_FINAL[numberstack_index - 1]));
    bring_stack_down(numberstack_index);
    //print_numberstack();
  }
}

void bring_stack_down(byte* pop_at_this_x) {
  for ( byte m = pop_at_this_x ; m < (sizeof(numberStack_FINAL) / (sizeof(BigNumber))); m++) {
    ////Serial.println("pop_at_this_x: " + String(m) + "  (sizeof(numberStack_FINAL/sizeof(BigNumber)):  " + String((sizeof(numberStack_FINAL) / (sizeof(BigNumber)))));
    ////Serial.println("numberStack_FINAL[m]: " + String(numberStack_FINAL[m]) + " numberStack_FINAL[m + 1]: " + String(numberStack_FINAL[m + 1]));
    if (m != (sizeof(numberStack_FINAL) / (sizeof(BigNumber)))) {
      numberStack_FINAL[m] = numberStack_FINAL[m + 1];
    } else numberStack_FINAL[m] = 0;
  }
  delete_ones++;
}

//void print_numberstack() {
//  //Serial.print("Numberstack: ");
//  for ( int t = 0; t < (sizeof(numberStack_FINAL) / sizeof(BigNumber)); t++) {
//    //Serial.print(String(numberStack_FINAL[t]) + " ");
//  } //Serial.println(" ");
//}


void evaluate_postfix() {
  for ( byte p = 0 ; p <= ((sizeof(postfix_stack_reference) - 1) / sizeof(byte)) ; p++ ) {
    byte value = postfix_stack_reference[p];
    if (value > 1) {
      perform_operation(value, p);
    }
  }
  Serial.print("Finished Processing, we got result approximate : ");
  Serial.print("thenumber");
  //  Serial.println(numberStack_FINAL[0]);
  //  print_numberstack();
}

