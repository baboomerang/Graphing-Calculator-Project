void calculate(char (&input)[INPUT_SIZE]) {
  String num;
  num.reserve(INPUT_SIZE); //prevents heap fragmentation
  Stack<char> opstack(INPUT_SIZE / 2); //binary operators need two operands
  List<BigNumber> operands;

  for (int x = 0; x <= index; x++) {
    char token = input[x];
    switch (token) {
      case '0'...'9':
      case '.':
        num.concat(token);
        break;
      case '+':
      case '-':
      case '*':
      case '/':
      case '^':
        if (num)
          transfer(num, operands);

        while (precedence(token) <= precedence(opstack.peek())) {
          if (associativity(token))
            break;
          doMath(opstack.pop(), operands);
        }
      case '(':
        opstack.push(token);
        break;
      case ')':
        while (opstack.peek() && opstack.peek() != '(') {
          doMath(opstack.pop(), operands);
        }
        opstack.pop(); //discard closing parenth
        break;
    }
  }

  // process entire opstack at the end of input stream
  while (opstack.peek() && opstack.peek() != '(') {
    doMath(opstack.pop(), operands);
  }

  //END OF ALGORITHM, CHECK STATUS OF RESULTS HERE

  if (opstack.peek() == '(') {
    raise(4);   //extra closing parenth, parenth mismatch
    return;
  }

  if (!opstack.isEmpty()) {
    raise(2);  //opstack should have been empty if syntax was respected
    return;
  }

  if (operands.count() != 1) {
    raise(3);  //if parenthesis were abused, there werent enough operators
    return;
  }

  auto final = operands.pop();
  printBignum(final);
}

void transfer(String &num, List<BigNumber> &output) {
  char array[num.length() + 1];
  num.toCharArray(array, num.length() + 1);
  output.push(BigNumber(array));
  num.remove(0);
}

inline byte doMath(char op, List<BigNumber> &output) {
  if (output.count() < 2)
    return 0;  //if there arent enough operands, return error

  BigNumber y = output.pop();
  BigNumber z = output.pop();
  switch (op) {
    case ('+'):
      z += y;
      break;
    case ('-'):
      z -= y;
      break;
    case ('*'):
      z *= y;
      break;
    case ('/'):
      z /= y;
      break;
    case ('^'):
      z = z.pow(y);
      break;
    default: //defense against a dumb anti-pattern I accidentally made
      output.push(z);
      output.push(y);
      return 0; //if invalid op, put operands back in their place
  }

  output.push(z);
  return 1;
}
bool isOperator(char &op) {
  switch (op) {
    case '+':
    case '-':
    case '*':
    case '/':
    case '^':
      return true;
    default:
      return false;
  }
}
byte precedence(char &op) {
  switch (op) {
    case '+':
    case '-':
      return 1;
    case '*':
    case '/':
      return 2;
    case '^':
      return 3;
    default:
      return 0;
  }
}
bool associativity(char &op) {
  if (op == '^') return 1;
  return 0;
}
