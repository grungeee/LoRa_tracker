#ifndef PFOD_LINKEDLIST_H_
#define PFOD_LINKEDLIST_H_
// pfodLinkedList.h
/*
   Revised 2022/09/26 to leave iterator unchanged except for remove() and getFirst()/getNext()
   
   Modified by Matthew Ford to remove index and only use pointers and add getFirst(), getNext() iterator
   Now acts as LOFO list
   NULL data pointers not added to list
   Not longer limited to 255 elements. Only limited by available memory. Still no caching
   !! NOTE CAREFULLY !! This version pfodLinkedList DOES NOT call delete() on the data pointers so the call must clean up if necessary

  (c)2022 Forward Computing and Control Pty. Ltd.
  This code is not warranted to be fit for any purpose. You may only use it at your own risk.
  These modifications may be freely used for both private and commercial use subject to the included LICENSE file
  Provide this copyright is maintained.
*/

/*

  Light-weight implementation of LinkedList, appropriate for use in Arduino and other memory-critical environments.

  - Handling pointers, rather than copies of actual contained objects.
  - Up to 255 entries
  - No last element pointer
  - No sequential get-cache
  - No support for sorting

  This object consumes just 5 bytes per instance + 4 per node, making itself more appropriate for use in memory-critical
  environments. Since the class is targeted for use with small lists with up to a few dozens of entries, the optimization
  cuts are not significantly affecting the performance.

  Based on LinkedList library by ivanseidel (https://github.com/ivanseidel/LinkedList).

   Created on: 31. sij 2017.
       Author: JonnieZG
*/

#include <stddef.h>

template<class T>
struct pfodListNode {
  T *data;
  pfodListNode<T> *next;
};

template<typename T>
class pfodLinkedList {

  protected:
    pfodListNode<T> *root;
    pfodListNode<T> *current; // for list tranversals
    size_t count; // the number of items in the list

  public:
    pfodLinkedList();
    virtual ~pfodLinkedList(); // calls clear to release the list containers, DOES NOT call delete on data so caller needs to clean up if necessary
    /*
        The size of the list
        @ret - the current number of elements in the list
    */
    virtual size_t size();
    
    /*
        Adds this data pointer at the front of the list
        NULL data pointers not added to list
        current iterator not changed, call getFirst() to include this new element
        @ret - true if added, else false if data pointer NULL or out of memory
    */
    virtual bool add(T*);

    /*
        Adds this data pointer at the end of the list
        NULL data pointers not added to list
        current iterator not changed
        @ret - true if added, else false if data pointer NULL or out of memory
    */
    virtual bool append(T* _t);

    /*
       returns the index of data pointer or size() if not found
       current iterator is NOT changed
      @ret - index of this data, or size() if not found
    */
    virtual size_t getIndex(T*);

    /*
       returns the data pointer if the current iterator, can be NULL
       use to mark current position and then reset later using
       setCurrentIterator( )
      @ret - NULL or point to list data
    */
    virtual T* getCurrentIterator();

    /*
      sets list iterator to this data item
      getNext() will get the next item
      If arg is NULL just set current iterator to NULL
      If the dataItem is not longer on the list the current iterator is set to NULL
      @ret - true if _current still in list
    */
    virtual bool setCurrentIterator(T* _current);

    /*
       Removes this data pointer
       current iterator is set to NULL if its item is removed
      @ret - NULL if data pointer not found on the list, else the removed data element pointer
    */
    virtual T* remove(T*);

    /*
       Removes this data pointer
       current iterator is set to NULL if its item is removed
      @ret - NULL if idx not found on the list, else the removed data element
    */
    virtual T* remove(size_t idx);

    /*
       inserts this data pointer at the idx, if idx >= size() append at end
       current iterator is not changed
      @ret - true if inserted, else false
    */
    virtual bool insertAt(T*, size_t idx);

    /*
       Removes first element
       Use this repeatedly to clear list.
       You need to free() the data pointer returned
       current iterator is set to NULL if its item is removed
      @ret - NULL if list empty, else first data element pointer
    */
    virtual T* remove();

    /*
      get the root of the list
      Sets current interator to root
      @ret - NULL at end of list, else first data element pointer
    */
    virtual T* getFirst();

    /* get next element in list
      using current interator
      @ret - NULL at end of list, else next element
    */
    virtual T* getNext();

    /* get idx  element in list
       current iterator is NOT changed
      @ret - NULL if idx >= size()
    */
    virtual T* get(size_t idx);

    /*
      This is also called by the destructor when the list goes out of scope!!
      !! NOTE CAREFULLY !! The destructor and clear() DOES NOT call delete() on the data pointers so caller needs to clearn up if necessary
      Using pointers returned from malloc etc will most likely crash when clear() is called.
    */
    virtual void clear();

};

// pfodLinkedList.cpp
/*
   Modified by Matthew Ford to remove index and only use pointers and add getFirst(), getNext() iterator
   Now acts as LOFO list
   NULL data pointers not added to list
   Not longer limited to 255 elements. Only limited by available memory. Still no caching
      !! NOTE CAREFULLY !! The destructor and clear() DOES NOT call delete() on the data pointers so caller needs to clearn up if necessary

  (c)2022 Forward Computing and Control Pty. Ltd.
  This code is not warranted to be fit for any purpose. You may only use it at your own risk.
  These modifications may be freely used for both private and commercial use subject to the included LICENSE file
  Provide this copyright is maintained.
*/

/*

  Light-weight implementation of LinkedList, appropriate for use in Arduino and other memory-critical environments.

  - Handling pointers, rather than copies of actual contained objects.
  - Up to 255 entries
  - No last element pointer
  - No sequential get-cache
  - No support for sorting

  This object consumes just 5 bytes per instance + 4 per node, making itself more appropriate for use in memory-critical
  environments. Since the class is targeted for use with small lists with up to a few dozens of entries, the optimization
  cuts are not significantly affecting the performance.

  Based on LinkedList library by ivanseidel (https://github.com/ivanseidel/LinkedList).

   Created on: 31. sij 2017.
       Author: JonnieZG
*/

// ------------ Template Implementation ------------

#include "pfodLinkedList.h"

template<typename T>
pfodLinkedList<T>::pfodLinkedList() {
  root = NULL;
  current = NULL;
  count = 0;
}

/*
  calls clear to release all data as well as the list containers
*/
template<typename T>
pfodLinkedList<T>::~pfodLinkedList() {
  clear();
}

/*
    The size of the list
    @ret - the current number of elements in the list
*/
template<typename T>
size_t pfodLinkedList<T>::size() {
  return count;
}

/*
    Adds this data pointer at the front of the list
    NULL data pointers not added to list
    current iterator not changed, call getFirst() to include this new element
    @ret - true if added, else false if data pointer NULL or out of memory
*/
template<typename T>
bool pfodLinkedList<T>::add(T* _t) {
  if (_t == NULL) {
    return false;
  }
  size_t idx = getIndex(_t);
  if (idx != size()) {
    return false; // already on list
  }
  pfodListNode<T> *tmp = new pfodListNode<T>();
  if (tmp == NULL) {
    return false;
  }
  tmp->data = _t;
  tmp->next = root;
  root = tmp;
  count++;
  return true;
}

/*
    Adds this data pointer at the end of the list
    NULL data pointers not added to list
    current iterator not changed
    @ret - true if added, else false if data pointer NULL or out of memory
*/
template<typename T>
bool pfodLinkedList<T>::append(T* _t) {
  if (_t == NULL) {
    return false;
  }
  size_t idx = getIndex(_t);
  // if size() == 0, then not on list
  // then idx will be returned as 0 so size()==idx
  if (idx != size()) {
    return false; // already on list
  }
  // else not on list or list empty
  pfodListNode<T> *tmp = new pfodListNode<T>();
  if (tmp == NULL) {
    return false;
  }
  tmp->data = _t;
  if (root == NULL) {
    root = tmp;
  } else {
    pfodListNode<T> *currentPtr = root;
    while (currentPtr->next != NULL) {
      currentPtr = currentPtr->next;
    }
    currentPtr->next = tmp;
  }
  count++;
  return true;
}

/*
   Removes this data pointer
   current iterator is set to NULL if its item is removed
  @ret - NULL if data pointer not found on the list, else the removed data element pointer
*/
template<typename T>
T* pfodLinkedList<T>::remove(T * _t) {
  if ((root == NULL) || (_t == NULL)) {
    return NULL;
  }
  T* rtnData = NULL;
  pfodListNode<T> *toDelete = NULL;
  if (root->data == _t) {
    toDelete = root;
    root = toDelete->next;
    rtnData = toDelete->data;
    if (current == toDelete) {
       current = NULL; 
    }
    delete(toDelete);
    if (count >= 1) {
      count--;
    }
    return rtnData;
  }
  // else not root
  pfodListNode<T>* lastListPtr = root;
  pfodListNode<T>* listPtr = root->next;
  while (listPtr) {
    if (listPtr->data == _t) {
      // found it
      toDelete = listPtr;
      lastListPtr->next = toDelete->next;
      rtnData = toDelete->data;
      if (current == toDelete) {
        current = NULL; // so getNext() returns next element in list
      }
      delete(toDelete);
      if (count >= 1) {
        count--;
      }
      return rtnData;
    }
    // else get next
    lastListPtr = listPtr;
    listPtr = listPtr->next;
  }
  return rtnData; // NULL; // null not found
}

/*
   returns the index of data pointer or size() if not found
   current iterator is NOT updated
  @ret - index of this data, or size() if not found
*/
template<typename T>
size_t pfodLinkedList<T>::getIndex(T * _t) {
  if ((root == NULL) || (_t == NULL)) {
    return size();
  }
  size_t idx = 0;
  pfodListNode<T>* listPtr = root;
  while (listPtr) {
    if (listPtr->data == _t) {   // found it
      return idx;
    }
    idx++;
    listPtr = listPtr->next;
  }
  return size();    // not found
}

/*
   returns the data pointer if the current iterator, can be NULL
   use to mark current position and then reset later using
   setCurrentIterator( ) to restore it
   @ret - NULL or point to list data
*/
template<typename T>
T* pfodLinkedList<T>::getCurrentIterator() {
  if (current) {
    return current->data;
  } //
  return NULL;
}

/*
   sets list iterator to this data item
   getNext() will get the next item
   If arg is NULL just set current iterator to NULL
   If the dataItem is not longer on the list the current iterator is set to NULL
*/
template<typename T>
bool pfodLinkedList<T>::setCurrentIterator(T* _current) {
  current = NULL;
  if ((root == NULL) || (_current == NULL)) {
    return false;
  }
  pfodListNode<T>* listPtr = root;
  while (listPtr) {
    if (listPtr->data == _current) {   // found it
      current = listPtr;
      return true;
    }
    listPtr = listPtr->next;
  }
  // not found leave as NULL
  return false;
}

/*
   Removes this data pointer
   current iterator is set to NULL if its item is removed
  @ret - NULL if idx not found on the list, else the removed data element
*/
template<typename T>
T* pfodLinkedList<T>::remove(size_t idx) {
  if ((root == NULL) || (idx >= size())) {
    return NULL;
  }
  T* rtnData = NULL;
  rtnData = get(idx); // can be null
  return remove(rtnData); // handles null // updates size() // deletes linklist obj but not the data
}

/*
   inserts this data pointer at the idx, current idx is pushed to idx+1
   if idx >= size() append at end
   current iterator is not changed
  @ret - true if inserted, else false if already in list or memory alloc failed
*/
template<typename T>
bool pfodLinkedList<T>::insertAt(T* _t, size_t idx) {
  size_t existingIdx = getIndex(_t); // can be null
  if (existingIdx != size()) { // works for empty list as well empty list returns 0 (size())
    return false; // already on list
  }
  // else find idx and insert there
  if (idx >= size()) {
    return append(_t); // at end
  }
  if (idx == 0) {
    return add(_t);
  }
  pfodListNode<T> *tmp = new pfodListNode<T>();
  if (tmp == NULL) {
    return false;
  }
  tmp->data = _t;

  // else get idx-1 to position current
  size_t idx_1 = idx - 1; // idx >0 from above
  pfodListNode<T>* currentPtr = root;
  size_t count = 0;
  while ((count < idx_1) && (currentPtr != NULL)) {
    currentPtr = currentPtr->next;
    count++;
  }
  // currentPtr is idx-1
  pfodListNode<T>* lastListPtr = currentPtr;
  pfodListNode<T>* nextListPtr = currentPtr->next;
  // insert
  lastListPtr->next = tmp;
  tmp->next = nextListPtr;
  count++;
  return true;
}

/*
   Removes first element
   Use this repeatedly to clear list.
   You need to free() the data pointer returned
   current iterator is set to NULL if its item is removed
  @ret - NULL if list empty, else first data element pointer
*/
template<typename T>
T* pfodLinkedList<T>::remove() {
  if (root == NULL) {
    return NULL;
  }
  T* rtnData = NULL;
  pfodListNode<T> *toDelete = NULL;
  toDelete = root;
  root = toDelete->next;
  rtnData = toDelete->data;
  if (current == toDelete) {
    current = NULL;
  }
  delete(toDelete);
  if (count >= 1) {
    count--;
  }
  return rtnData;
}

/*
    get the root of the list
    Sets current interator to root
    @ret - NULL at end of list, else first data element pointer
*/
template<typename T>
T* pfodLinkedList<T>::getFirst() {
  current = root;
  if (current) {
    return (current->data);
  } // else
  return NULL;
}

/* get next element in list
  using current interator
  @ret - NULL at end of list, else next element
*/
template<typename T>
T* pfodLinkedList<T>::getNext() {
  if (current == NULL) {
    return NULL;
  } // else
  current = current->next;
  if (current) {
    return (current->data);
  } // else
  return NULL;
}

/* get idx  element in list
  current iterator is NOT changed
  @ret - NULL if idx >= size()
*/
template<typename T>
T* pfodLinkedList<T>::get(size_t idx) {
  pfodListNode<T>* currentPtr = root;
  if (currentPtr == NULL) {
    return NULL;
  } // else
  size_t count = 0;
  while ((count < idx) && (currentPtr != NULL)) {
    currentPtr = currentPtr->next;
    count++;
  }
  if (currentPtr) {
    return (currentPtr->data);
  } // else
  return NULL;
}

/*
  This is also called by the destructor when the list goes out of scope!!
  !! NOTE CAREFULLY !! The destructor and clear() now DOES NOT call delete() on the data pointers so call needs to clean up if necessary
*/
template<typename T>
void pfodLinkedList<T>::clear() {
  pfodListNode<T>* tmp;
  current = NULL;
  while (root != NULL) {
    tmp = root;
    root = root->next;
    //delete tmp->data; // must be class created with new ...
    // pfodLinkedList does not delete data
    tmp->data = NULL;
    delete tmp;
    tmp = NULL;
  }
  count = 0;
}

#endif /* PFOD_LINKEDLIST_H_ */
