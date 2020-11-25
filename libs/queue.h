#ifndef QUEUE_H
#define QUEUE_H

#include <string.h>

/*
 * This queue.h code is provided "as-is" with no warranty, implied or not.
 * By using this code you agree to proceed at your own risk.
 *
 * This Queue object is a STATIC array. It is a very simplified version with
 * a small code footprint with only the minimum required for the arduino.
 * It is designed specifically for standard types (byte, int, long, char)
 *
 * You can use this queue with constant size, user-defined types.
 * DO NOT USE strings or other types that dynamically reallocate memory in this
 *       -updated nov 22, 2020
 */

template<typename T>
class Queue {
private:
    int size;
    int rear;
    T* array;
public:
    Queue(const int);
    ~Queue();
    void erase(const int);
    void enqueue(const T);
    T dequeue();
    String toString() const;
    int count() const;
    bool isEmpty() const;
    bool isFull() const;
};


template<typename T>
Queue<T>::Queue(const int max) {
    size = max;
    rear = 0;
    array = new T[size]{0};
}

template<typename T>
Queue<T>::~Queue() {
    size = 0;
    rear = 0;
    delete[] array;
}


/* Erases all elements from given index to the end of array */
template<typename T>
void Queue<T>::erase(const int pos) {
    for (int x = pos; x < rear; x++) {
        array[x] = 0;
    }
    rear = pos;
}

/* Pushes a copy of an element to the array */
template<typename T>
void Queue<T>::enqueue(const T item) {
    if (isFull())
        return;  //silent defense

    array[rear++] = item;
}

/* Gets a copy and pops the first element in the array */
/* completes in O(n) time due to shifting */
template<typename T>
T Queue<T>::dequeue() {
    if (isEmpty())
        return T();  //silent defense

    T result = array[0];

    //shift every element one unit to the left
    for (int x = 0; x < rear; x++) {
        array[x] = array[x + 1];
    }
    rear--;

    return result;
}

/* Returns a copy of the internal array as a C++ string */
template<typename T>
String Queue<T>::toString() const {
    return String(array);
}

/* Get the length of the queue */
template<typename T>
int Queue<T>::count() const {
    return rear;
}

/* Check if the queue is empty */
template<typename T>
bool Queue<T>::isEmpty() const {
    return rear <= 0;
}

/* Check if the queue is full */
template<typename T>
bool Queue<T>::isFull() const {
    return rear >= size;
}

#endif