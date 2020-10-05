#ifndef STACK_H
#define STACK_H

/* 
This stack.h code is provided "as-is" with no warranty, implied or not.
By using this code you agree to proceed at your own risk.
*/

template<typename T>
class Stack {
private:
    int maxsize;
    int top;
    T* array;
public:
    Stack(const int max);
    ~Stack();
    void push(const T item);
    T pop();
    T& peek();
    int size() const;
    bool isEmpty() const;
    bool isFull() const;
};

template<typename T>
Stack<T>::Stack(const int max) {
    maxsize = max;
    top = 0;
    array = (T*) calloc(maxsize, sizeof(T));

    if (array == NULL)
	abort();
}

template<typename T>
Stack<T>::~Stack() {
    delete(array);
    array = NULL;  //resets pointer position, important
    top = 0;
    maxsize = 0;
}


template<typename T>
T& Stack<T>::peek() {
    return array[top - 1];
}

template<typename T>
void Stack<T>::push(const T item) {
    if (!isFull())
        array[top++] = item;
    else
        return;
}

template<typename T>
T Stack<T>::pop() {
    T result = NULL;

    if (!isEmpty()) {
        result = array[--top];  //must predecrement because 'top' points to next empty space
        array[top] = 0;
    }

    return result;
}

template<typename T>
int Stack<T>::size() const {
    return top;
}
template<typename T>
bool Stack<T>::isEmpty() const {
    return top < 1;
}
template<typename T>
bool Stack<T>::isFull() const {
    return top >= maxsize;
}

#endif
