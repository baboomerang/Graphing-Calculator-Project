#ifndef STACK_H
#define STACK_H

/*
 * This stack.h code is provided "as-is" with no warranty, implied or not.
 * By using this code you agree to proceed at your own risk.
 *
 * This Stack object is a STATIC array. It is a very simplified version with
 * a small code footprint with only the minimum required for the arduino.
 * It is designed specifically for standard types (byte, int, long, char)
 *
 * You can use this stack with constant size, user-defined types.
 * DO NOT USE strings or other types that dynamically reallocate memory in this
 *       -updated nov 13, 2020
 */

template<typename T>
class Stack {
private:
    int size;
    int top;
    T* array;
public:
    Stack(const int);
    ~Stack();
    void push(const T);
    T pop();
    T peek();
    int count() const;
    bool isEmpty() const;
    bool isFull() const;
};

/* 
 * This stack class uses the empty-stack convention
 * so `int top` points to the next empty slot in the stack
 * `top` increments after a push and decrements before a pop
 */

template<typename T>
Stack<T>::Stack(const int max) {
    size = max;
    top = 0;
    array = new T[size]{0};
}

template<typename T>
Stack<T>::~Stack() {
    size = 0;
    top = 0;    
    delete[] array;
    array = NULL;
}

/* Send a copy of the data into the stack */
template<typename T>
void Stack<T>::push(const T item) {
    if (isFull())
        return;  //silent defense against stack overflow

    array[top++] = item;
}

/* Take the top element out of the stack */
template<typename T>
T Stack<T>::pop() {
   if (isEmpty())
       return T(); //silent defense against stack underflow

   T result = array[--top];
   array[top] = 0;
   return result;
}

/* Get a copy of the top element in the stack */
template<typename T>
T Stack<T>::peek() {
   if (isEmpty())
        return T(); //silent defense, same reason as pop

   return array[top - 1];
}

/* Get the number of elements in the stack */
template<typename T>
int Stack<T>::count() const {
    return top;
}

/* Check if the stack is empty */
template<typename T>
bool Stack<T>::isEmpty() const {
    return top <= 0;    //valid index values are 0 to (size-1)
}

/* Check if the stack is full */
template<typename T>
bool Stack<T>::isFull() const {
    return top >= size;  //valid index values are 0 to (size-1)
}

#endif
