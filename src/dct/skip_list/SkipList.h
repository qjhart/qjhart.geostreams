#ifndef SKIP_LIST
#define SKIP_LIST
#include <iostream>
#include <fstream>
#include "SkipNode.h"
#include "RandomHeight.h"
using namespace std ;

template <class Key, class Obj>
  class SkipList
  {
  public:
    SkipList(float,int,Key*);
    ~SkipList();

    bool insert(Key*, Obj*);
    bool remove(Key*);
    SkipNode<Key, Obj>* retrieve(Key*);

    bool removeWithKey (Key*) ;
    void printFwd () ;
    void printBck () ;

    bool isEmpty () ;
    
    // void dump(ofstream&);
    SkipNode<Key,Obj>* getHead () ;
    SkipNode<Key,Obj>* getTail () ;
    SkipNode<Key,Obj>* getNext (SkipNode<Key,Obj>* node) ;   
    SkipNode<Key,Obj>* getPrev (SkipNode<Key,Obj>* node) ;   

  private:
    SkipNode<Key,Obj>* head;
    SkipNode<Key,Obj>* tail;
    float probability;
    int maxHeight;
    int curHeight;
    RandomHeight* randGen;
  };

template <class Key, class Obj>
  SkipList<Key,Obj>::SkipList(float p, int m, Key* k)
  {
    curHeight = 1;
    maxHeight = m;
    probability = p;
    randGen = new RandomHeight(m,p);

    // Create head and tail and attach them
    head = new SkipNode<Key,Obj>(maxHeight);
    tail = new SkipNode<Key,Obj>(k, (Obj*) NULL, maxHeight);
    for ( int x = 1; x <= maxHeight; x++ ) {
        head->fwdNodes[x] = tail;
        tail->bckNodes[x] = head ;
    }
  }

template <class Key, class Obj>
  SkipList<Key,Obj>::~SkipList()
  {
    // Walk 0 level nodes and delete all
    SkipNode<Key,Obj>* tmp;
    SkipNode<Key,Obj>* nxt;
    tmp = head;
    while ( tmp )
    {
      nxt = tmp->fwdNodes[1];
      delete tmp;
      tmp = nxt;
    }

    delete randGen ;
  }

template <class Key, class Obj>
  bool SkipList<Key,Obj>::insert(Key* k, Obj* o)
  {
    int lvl = 0, h = 0;
    SkipNode<Key,Obj>** updateVec =
      new SkipNode<Key,Obj>* [maxHeight+1];
    SkipNode<Key,Obj>* tmp = head;
    Key* cmpKey;

    // Figure out where new node goes
    for ( h = curHeight; h >= 1; h-- )
    {
      cmpKey = tmp->fwdNodes[h]->getKey();
      while ( *cmpKey < *k )
      {
        tmp = tmp->fwdNodes[h];
        cmpKey = tmp->fwdNodes[h]->getKey();
      }
      updateVec[h] = tmp;
    }
    tmp = tmp->fwdNodes[1];
    cmpKey = tmp->getKey();

    // If dup, return false
    if ( *cmpKey == *k )
    {
      delete updateVec ;
      return false;
    }
    else
    {
      // Perform an insert
      lvl = randGen->newLevel();
//std::cout << "lvl=" << lvl << "\n" ;
      if ( lvl > curHeight )
      {
        for ( int i = curHeight + 1; i <= lvl; i++ )
          updateVec[i] = head;
        curHeight = lvl;
      }
      // Insert new element
      tmp = new SkipNode<Key,Obj>(k, o, lvl);
      for ( int i = 1; i <= lvl; i++ )
      {
        tmp->fwdNodes[i] = updateVec[i]->fwdNodes[i];
        updateVec[i]->fwdNodes[i]->bckNodes[i] = tmp ;
        updateVec[i]->fwdNodes[i] = tmp;
        tmp->bckNodes[i] = updateVec[i] ;
      }
    }

    delete updateVec ;
    return true;
  }


template <class Key, class Obj>
  bool SkipList<Key,Obj>::remove(Key* k)
  {
    SkipNode<Key,Obj>** updateVec =
      new SkipNode<Key,Obj>* [maxHeight+1];
    SkipNode<Key,Obj>* tmp = head;
    Key* cmpKey;

     // Find the node we need to delete
    for ( int h = curHeight; h > 0; h-- )
    {
      cmpKey = tmp->fwdNodes[h]->getKey();
      while ( *cmpKey < *k )
      {
        tmp = tmp->fwdNodes[h];
        cmpKey = tmp->fwdNodes[h]->getKey();
      }
      updateVec[h] = tmp;
    }
    tmp = tmp->fwdNodes[1];
    cmpKey = tmp->getKey();

    if ( *cmpKey == *k )
    {
      for ( int i = 1; i <= curHeight; i++ )
      {
        if ( updateVec[i]->fwdNodes[i] != tmp ) 
          break;
        updateVec[i]->fwdNodes[i] = tmp->fwdNodes[i];
        (tmp->fwdNodes[i])->bckNodes[i] = updateVec[i] ;
      }
      delete tmp;
      while ( ( curHeight > 1 ) &&
            ( ( head->fwdNodes[curHeight]->getKey()
                == tail->getKey() ) ) )
        curHeight--;
      delete[] updateVec ;
      return true;
    }
    else
    {
      delete[] updateVec ;
      return false;
    }
  }

template <class Key, class Obj>
  bool SkipList<Key,Obj>::removeWithKey (Key* k) {
    SkipNode<Key,Obj>** updateVec =
      new SkipNode<Key,Obj>* [maxHeight+1];
    SkipNode<Key,Obj>* tmp = head;
    Key* cmpKey;

     // Find the node we need to delete
    for ( int h = curHeight; h > 0; h-- )
    {
      cmpKey = tmp->fwdNodes[h]->getKey();
      while ( *cmpKey < *k )
      {
        tmp = tmp->fwdNodes[h];
        cmpKey = tmp->fwdNodes[h]->getKey();
      }
      updateVec[h] = tmp;
    }
    tmp = tmp->fwdNodes[1];
    cmpKey = tmp->getKey();

    if ( *cmpKey == *k )
    {
      for ( int i = 1; i <= curHeight; i++ )
      {
        if ( updateVec[i]->fwdNodes[i] != tmp ) 
          break;
        updateVec[i]->fwdNodes[i] = tmp->fwdNodes[i];
        (tmp->fwdNodes[i])->bckNodes[i] = updateVec[i] ;
      }
      delete tmp;
      while ( ( curHeight > 1 ) &&
            ( ( head->fwdNodes[curHeight]->getKey()
                == tail->getKey() ) ) )
        curHeight--;

      delete cmpKey ;

      delete[] updateVec ;
      return true;
    }
    else
    {
      delete[] updateVec ;
      return false;
    }
}

template <class Key, class Obj>
  SkipNode<Key, Obj>* SkipList<Key,Obj>::retrieve(Key* k)
  {
    int h = 0;
    SkipNode<Key,Obj>** updateVec =
      new SkipNode<Key,Obj>* [maxHeight+1];
    SkipNode<Key,Obj>* tmp = head;
    Key* cmpKey;

    // Find the key and return the node
    for ( h = curHeight; h >= 1; h-- )
    {
      cmpKey = tmp->fwdNodes[h]->getKey();
      while ( *cmpKey < *k )
      {
        tmp = tmp->fwdNodes[h];
        cmpKey = tmp->fwdNodes[h]->getKey();
      }
      updateVec[h] = tmp;
    }
    tmp = tmp->fwdNodes[1];
    cmpKey = tmp->getKey();
    if ( *cmpKey == *k ) {
      delete[] updateVec ;
      return tmp ; // tmp->getObj();
    }
    else {
      delete[] updateVec ;
      return (SkipNode<Key,Obj>*) NULL;
    }
  }

template <class Key, class Obj>
  void SkipList<Key,Obj>::printFwd()
  {
    SkipNode<Key,Obj>* tmp;

    tmp = head;
    while ( tmp != tail )
    {
      if ( tmp == head )
        cout << "There's the head node!" << endl << flush;
      else
        // Your key class must support "<<"
        cout << "Next node holds key: " << *(tmp->getKey()) << endl
           << flush;
      tmp = tmp->fwdNodes[1];
    }
    cout << "There's the tail node!" << endl << flush;
  }

template <class Key, class Obj>
  void SkipList<Key,Obj>::printBck()
  {
    SkipNode<Key,Obj>* tmp;

    tmp = tail;
    while ( tmp != head )
    {
      if ( tmp == tail )
        cout << "There's the tail node!" << endl << flush;
      else
        // Your key class must support "<<"
        cout << "Next node holds key: " << *(tmp->getKey()) << endl
           << flush;
      tmp = tmp->bckNodes[1];
    }
    cout << "There's the head node!" << endl << flush;
  }

template <class Key, class Obj>
SkipNode<Key,Obj>* SkipList<Key,Obj>::getHead () {
  /*
  if (head->fwdNodes[1] == tail) {
    return NULL ;
  }
  return head->fwdNodes[1] ;*/
  return head ;
}

template <class Key, class Obj>
SkipNode<Key,Obj>* SkipList<Key,Obj>::getNext (SkipNode<Key,Obj>* node) {
  if (node == tail) {
    return NULL ;
  }

  return node->fwdNodes[1] ;
}

template <class Key, class Obj>
SkipNode<Key,Obj>* SkipList<Key,Obj>::getTail () {
  /*
  if (tail->bckNodes[1] == head) {
    return NULL ;
  }
  return tail->bckNodes[1] ;*/
  return tail ;
}

template <class Key, class Obj>
SkipNode<Key,Obj>* SkipList<Key,Obj>::getPrev (SkipNode<Key,Obj>* node) {
  if (node == head) {
    return NULL ;
  }

  return node->bckNodes[1] ;
}

template <class Key, class Obj>
bool SkipList<Key,Obj>::isEmpty () {
  if (head->fwdNodes[1] == tail) {
    return true ;
  }

  return false ;
}

/*
template <class Key, class Obj>
  void SkipList<Key,Obj>::dump(ofstream& of)
  {
    SkipNode<Key,Obj>* tmp;

    tmp = head;
    while ( tmp != tail )
    {
      if ( tmp == head )
        of << "There's the head node!" << endl << flush;
      else
        // Your key class must support "<<"
        of << "Next node holds key: " << tmp->getKey() << endl
           << flush;
      tmp = tmp->fwdNodes[1];
    }
    of << "There's the tail node!" << endl << flush;
  }
*/
#endif //SKIP_LIST

