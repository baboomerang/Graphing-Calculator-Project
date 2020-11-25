#ifndef LIST_H
#define LIST_H

/* 
 * This list.h code is provided "as-is" with no warranty, implied or not.
 * By using this code you agree to proceed at your own risk.
 *
 * This List Object is a dynamically allocated Linked-List. It is a very simplified version with
 * a small memory footprint with only the minimum required for the arduino.
 * 	-updated nov 13, 2020
 */

template<typename T>
class List {
private:
    int length;
    struct Node {
        T* data;
        Node* next;
    };
    Node* head;
    Node* tail;
public:
    List();
    ~List();
    void push(T* item);
    T* pop();
    T* peek();
    int count() const;
    bool isEmpty() const;
};

template<typename T>
List<T>::List() {
    length = 0;
    head = NULL;
    tail = NULL;
}

template<typename T>
List<T>::~List() {
    for (Node* iterator = head; head != NULL; iterator = head) {
        head = head->next;
        delete(iterator->data);  //call destructor on data
        delete(iterator);        //destruct the struct and pointers
    }
    tail = NULL;
    length = 0;
}

/* Create a new node with data and link it to the list */
template<typename T>
void List<T>::push(T* item) {
    if (head == NULL) {
        head = new Node{item, NULL};
        tail = head;
    } else {
        tail->next = new Node{item, NULL};
        tail = tail->next;
    }
    length++;
}

/* Copy data from tail node and free the tail node */
template<typename T>
T* List<T>::pop() {
    if (head == NULL)
        return T();

    //iterate and find the second last node
    Node* iterator = head;
    while (iterator->next != NULL) {
        if (iterator->next->next == NULL)
            break;
        iterator = iterator->next;
    }

    //copy data pointer from tail and pop the tail node
    T* result;
    if (length == 1) {  //if node is just the head
        result = iterator->data;
        delete(iterator);
    } else {
        result = iterator->next->data;
        delete(iterator->next);
    }

    //make second last node the new tail
    tail = iterator;
    tail->next = NULL;
    length--;
    return result;
}

/* Return a copy of the data ptr from tail node ONLY */
template<typename T>
T* List<T>::peek() {
    if (isEmpty())
        return T();  //silent defense against NULL pointer

    return tail->data;
}

/* Find how many elements are in the list */
template<typename T>
int List<T>::count() const {
    return length;
}

/* Check if the list is empty */
template<typename T>
bool List<T>::isEmpty() const {
    return length < 1;
}

#endif
