#include <iostream>
#include "List.h"
#include "Token.h"

template <typename T>
ListNode<T>::ListNode(T* _data) {
  data = _data;
  prev = nullptr;
  next = nullptr;
}

template <typename T>
ListNode<T>::~ListNode() {
  delete data;
}

template <typename T>
List<T>::List() {
  head = nullptr;
  tail = nullptr;
  size = 0;
}

template <typename T>
List<T>::~List() {
  if (head == nullptr) return;

  ListNode<T>* curr = head;
  ListNode<T>* temp;

  while (curr != tail) {
    temp = curr->next;
    delete curr;
    curr = temp;
  }
  delete curr;
}

template <typename T>
void List<T>::append(ListNode<T>* node) {
  size++;
  if (head == nullptr) {
    head = tail = node;
  }
  else {
    node->prev = tail;
    tail = tail->next = node;
  }
}

template <typename T>
void List<T>::display() {
  std::printf("Length of list: %d\n", size);
  if (head == nullptr) return;

  ListNode<T>* curr = head;
  while (curr) {
    curr->data->display();
    curr = curr->next;
  }
}

template struct ListNode<Token>;
template struct List<Token>;
