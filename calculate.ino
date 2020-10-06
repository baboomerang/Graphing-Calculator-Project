void calculate(char (&input)[INPUT_SIZE]) {
  String num;
  num.reserve(INPUT_SIZE);
  Stack<char> opstack(INPUT_SIZE / 2);  //binary operators require 2 operands
  List<BigNumber> operands;

  for (int x = 0; x <= index; x++) {
    char token = input[x];
    if (isDigit(token) || token == '.') {
      num.concat(token);
    } else {
      if (num)
        transfer(num, operands);

      if (isOperator(token)) {
        while (precedence(token) <= precedence(opstack.peek())) {
          if (associativity(token))
            break;  //warning: clever use of break; only when '^' is pushed to another '^'
          doMath(opstack.pop(), operands); //must do math with higher precedence operators first
        }
        opstack.push(token);
      }

      if (token == '(')
        opstack.push(token);

      if (token == ')') {
        while (doMath(opstack.peek(), operands) == 0)
          opstack.pop();
      }

    }
  }

  //above logic only runs while theres characters to read.
  //we must do one final check at the end of the input stream to get proper output

  while (doMath(opstack.peek(), operands) == 0) {
    opstack.pop();  //if do math was a success, remove it from the stack.
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

inline uint8_t doMath(char op, List<BigNumber> &output) {
  if (output.count() < 2) {
    return 1;  //if there arent enough operands, return error
  }
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
      z.pow(y);
      break;
    default:
      return 1;  //catch all if parenths get caught in the case
  }
  output.push(z);
  return 0;
}
bool isOperator(char &value) {
  switch (value) {
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
byte associativity(char &op) {
  if (op == '^') return 1;
  return 0;
}
