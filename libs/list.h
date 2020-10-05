#ifndef LIST_H
#define LIST_H

/* 
This list.h code is provided "as-is" with no warranty, implied or not.
By using this code you agree to proceed at your own risk.
*/

template<typename T>
class List {
private:
    int length;
    struct Node {
        T data;
        Node *next;
    };
    Node *head;
    Node *tail;
public:
    List();
    ~List();
    void push(const T item);
    T pop();
    T& peek();
    int count();
    bool isEmpty();
};

template<typename T>
List<T>::List() {
    length = 0;
    head = NULL;
    tail = NULL;
}

template<typename T>
List<T>::~List() {
    Node *iterator;
    while (head != NULL) {
        iterator = head;
        head = head->next;
        delete(iterator);
    }
    head = NULL;
    tail = NULL;
    length = 0;
}


template<typename T>
void List<T>::push(const T item) {
    Node *tmp = new Node;  //node constructor
    tmp->data = item;
    tmp->next = NULL;

    if (head == NULL) {    //if empty list, initialize the head
        head = tmp;
        tail = tmp;
    } else {               //else make the current tail point to new tail (tmp)
        tail->next = tmp;
        tail = tmp;
    }
    ++length;
}

template<typename T>
T List<T>::pop() {
    if (length < 1) {     //pop NULL if List is empty
        return NULL;
    }

    if (length == 1) {    //pop the head if only head
        --length;
        T result = head->data;
        delete(head);
        head = NULL;
        tail = NULL;
        return result;
    }
    
                    //else iterate and find the item right before the last
    Node *it = head;
    while (it->next->next != NULL) {
        it = it->next;
    }

    --length;
    T result = it->next->data;
    delete(it->next);
    it->next = NULL;
    return result;
}

template<typename T>
T& List<T>::peek() {
    return tail->data;
}

template<typename T>
int List<T>::count() {
    return length;
}

template<typename T>
bool List<T>::isEmpty() {
    return length < 1;
}

#endif