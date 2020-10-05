void calculate(char (&input)[INPUT_SIZE]) {
  String num;
  num.reserve(INPUT_SIZE);
  Stack<char> opstack(INPUT_SIZE / 2);
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
          if (operands.count() >= 2)
            doMath(opstack.pop(), operands); //must do math with higher precedence operators first
        }
        opstack.push(token);
      }

      if (token == '(') {
        opstack.push(token);
      }

      if (token == ')') {
        for (char token = opstack.pop(); token != (NULL || '('); token = opstack.pop()) {
          if (operands.count() >= 2)
            doMath(token, operands);
        }
      }

    }
  }

  while (!opstack.isEmpty()) {
    if (operands.count() >= 2)
      doMath(opstack.pop(), operands);
    else
      break;  //if operand stack is too small, break the infinite loop
  }

  if (!opstack.isEmpty())
    raise(2);  //opstack should have been empty if syntax was respected
  if (operands.count() != 1)
    raise(3);  //if parenthesis were abused, we would get these errors

  auto final = operands.pop();
  printBignum(final);
}

void transfer(String &num, List<BigNumber> &output) {
  char array[num.length() + 1];
  num.toCharArray(array, num.length() + 1);

  output.push(BigNumber(array));
  num.remove(0);
}

void doMath(char op, List<BigNumber> &output) {
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
  }
  output.push(z);
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
