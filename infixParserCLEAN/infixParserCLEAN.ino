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

//infixnumberstack and the Cin String
char infixRAWnumberStack[100];  //string of all the numbers together.
char infixstring[1000];
//processed stacks of information
int infix_stack_reference[50]; // reference key showing INFIX notation of the expression in a simplified view
int postfix_stack_reference[50]; // reference key showing POSTFIX notation of the expression in a simplified view
int postfix_opstack[50]; // a stack used for rearranging operators to get them in PEMDAS order.
long numberStack_FINAL[26]; // where operands are stored by index nmbrstack_FINAL[16] = "2932.231153" for example.

//count of how many open parenthesis are in the expression
unsigned int openparenth_count = 0;
//this is an immediate
bool active_parenth = false;

void setup() {
  infixstring[0] = '(';
  infX = 1;
  Serial.begin(9600);
}
//the whole code is nested under serialdataPull();
void loop() {
  serialdataPull();
}

void doNothing() {
  active_parenth = false;
}

void serialdataPull() {
  if (Serial.available() > 0) { //make sure serial is open Cin
    int incomingByte = Serial.read();
    byteChar = char(incomingByte);
    byteChar != 'g' ? infixstring[infX] = byteChar : infixstring[infX - 1] = infixstring[infX - 1];
    infX += 1;
    //=============== INFIXPARSER ========
    infixproc();
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
          Serial.println("num_x : " + String(num_x) + " | " + "parenthcount : " + String(openparenth_count) + " | " + "cutHere : " + String(cutHere) + " | ");
          openparenth_count -= 1;
          active_parenth = true;
          if ( cutHere != num_x ) {
            cutHere = num_x;
            Serial.println("DEBUG: save num INVOKED BY CLOSING PARENTHESIS");
            save_num(start, cutHere, num_indx);
          }
          save_op(7); // this line IS VERY ORDER SENSITIVE, DO NOT CHANGE THE ORDER OTHERWISE STUFF BREAKS. THIS LINE HAS TO BE HERE
          if (openparenth_count == 0) {
            Serial.println("PARENTH COUNT IS 0");
            syntax_check(i); //=================================================== as a terrible design choice, this whole program continues by calling the syntax check soubroutine
          }
          break;
        case '^':
          //           if (cutHere == num_x) {
          //             Serial.println("doubleoperatorerror");
          //             Serial.println("aborting.......");
          //             abort();
          //           } else {
          cutHere = num_x;
          active_parenth == false ? save_num(start, cutHere, num_indx) : doNothing();
          save_op(8);
          start = cutHere;
          //           }
          break;
         case '-':
          //           if (cutHere == num_x) {
          //             Serial.println("doubleoperatorerror");
          //             Serial.println("aborting.......");
          //             abort();
          //           } else {
          cutHere = num_x;
          active_parenth == false ? save_num(start, cutHere, num_indx) : doNothing();
          save_op(2);
          start = cutHere;
          //           }
          break;
        case '+':
          //           if (cutHere == num_x) {
          //             Serial.println("doubleoperatorerror");
          //             Serial.println("aborting.......");
          //             abort();
          //           } else {
          cutHere = num_x;
          active_parenth == false ? save_num(start, cutHere, num_indx) : doNothing();
          save_op(3);
          start = cutHere;
          //           }
          break;
        case '*':
          //           if (cutHere == num_x) {
          //             Serial.println("doubleoperatorerror");
          //             Serial.println("aborting.......");
          //             abort();
          //           } else {
          cutHere = num_x;
          Serial.println("SAVENUMINVOKED BY MULTIPLICATION");
          active_parenth == false ? save_num(start, cutHere, num_indx) : doNothing();
          save_op(4);
          start = cutHere;
          //           }
          break;
        case '/':
          //           if (cutHere == num_x) {
          //             Serial.println("doubleoperatorerror");
          //             Serial.println("aborting.......");
          //             abort();
          //           } else {
          cutHere = num_x;
          active_parenth == false ? save_num(start, cutHere, num_indx) : doNothing();
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

  } //end of if character g
} //end of infixproc

void save_num(int start, int cutpoint, int index_xpos) {
  save_op(1);
  String Z = String(infixRAWnumberStack);
  String Zshort = Z.substring(start, cutpoint);
  //the cutpoint does not include the value at the cutpoint. its an exclusion limit. if we had 97806 and cut point was 4 digits with start 0, we would just get the first 3 (978)
  long i = Zshort.toInt();
  numberStack_FINAL[index_xpos] = i;
  num_indx++;
  //numberStack_FINAL IS WHERE ALL THE FULL NUMBERS ARE STORED.  numberStack_FINAL[x] = "23223" numberStack_FINAL[x+1] = "3567" .... etc
  Serial.println("OUTPUT: we have this for Z / infixRAWnumberStack simplified " + String(Z) + "    OUTPUT: we have this for individual i.shortstring saved " + String(i));
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
      /* we know that the infix notation expression will always end with a 1,
        so by piggybacking on that predictable ending, we can tie together
        a final check to see if its actually the end of the string*/
      if ( infix_stack_reference[infix_key_x + 1] == 0 ) { // WARNING!!!!!!!!!!!!!!!!! this code might potentially be problematic if we have infix expressions that end in parenthesis.
        //        //======== Operator Stack Popping Code
        //        for (int p = ((sizeof(postfix_opstack) - 1) / sizeof(int)) ; p >= 0 ; p--) {
        //          int opr8tr = postfix_opstack[p];
        //          if ( opr8tr != 0 ) { // this for loop essentially pops the stack from top to the bottom in descending order. if any alignment errors, the != 0 mediates any small calibraton issues
        //            copy(numberrepeat, opr8tr);
        //          }
        //        }
        //        //=====================
      }
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
  } // prints the postfix stack reference when we reach end of string.
  for ( int p = ((sizeof(postfix_stack_reference) - 1) / sizeof(int)) ; p >= 0 ; p--) {
    Serial.println("postfix_stack_reference[" + String(p) + "] =  " + String(postfix_stack_reference[p]));
  }
}

void copy(byte location, byte input ) {
  postfix_stack_reference[location] = input;
  numberrepeat++;
}

void pushtostack(byte precedence, int opr8tr) {
  delay(50);
  for (int i = 0; ( i < (sizeof(postfix_opstack) / sizeof(int))); i++) {
    Serial.print(String(postfix_opstack[i]));
  } Serial.println("");
  delay(50);
  Serial.println("precedence: " + String(precedence) + " operator value: " + String(opr8tr));
  //some funky stuff with checking p = 1 and it wont override it at all due to some funky stuff. Any value after p == 1 breaks the code chain and dupes the top op code for some stupid reason. go fix that bro
  for ( int p = ((sizeof(postfix_opstack) - 1) / sizeof(int)) ; p >= 0 ; p--) {

    if ( p == 0 && postfix_opstack[p] == 0 ) { //this would set the first operator into the stack considering its all 0's first and we have to make sure its the bottom one.
      postfix_opstack[p] = opr8tr;
      //      Serial.println("location is  " + String(p) + " operator value: " + String(postfix_opstack[p]));
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
    } else if ((opr8tr != 7 && opr8tr > postfix_opstack[p] && postfix_opstack[p] != 0 && postfix_opstack[p] != 6))  {
      postfix_opstack[ p + 1 ] = opr8tr;
      break;
    }
    else if ((opr8tr != 7 && opr8tr <= postfix_opstack[p] && postfix_opstack[p] != 0 && postfix_opstack[p] == 6))  {
      postfix_opstack[ p + 1 ] = opr8tr;
      break;
    } else if (precedence == 255 && opr8tr == 7) {
      if (postfix_opstack[p] != 0) {
        Serial.println("we have for p: " + String(p) + "   " + "for postfix_opstack[pp], we have : " + String(postfix_opstack[p]));
        int operator_2b_popped = postfix_opstack[p];
        postfix_opstack[p] = 0;
        if (operator_2b_popped != 6) {
          copy(numberrepeat, operator_2b_popped);
        } else break;
      }
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