#ifndef __LIST_H__
#define __LIST_H__

#include <iostream>

template <typename T>
struct ListNode {
  T* data;
  ListNode* prev;
  ListNode* next;
  ListNode(T* _data);
  ~ListNode();
};

template <typename T>
struct List {
  ListNode<T>* head;
  ListNode<T>* tail;
  int size;
  List();
  ~List();
  void append(ListNode<T>* node);
  void display();
};

#endif
