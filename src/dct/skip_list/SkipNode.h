#ifndef SKIP_LIST_NODE
#define SKIP_LIST_NODE

struct Product
{
  float cost;
  int   quantity;
  int   location;
};

typedef Product productData;

template <class Key, class Obj>
  class SkipList;

template <class Key, class Obj>
  class SkipNode
  {
  public:
  //  friend class SkipList;
    SkipNode(Key*, Obj*, int);
    SkipNode(int);
    ~SkipNode();

    Key* getKey(void);
    Obj* getObj(void);
    int   getHgt(void);

    SkipNode* next () ;
    SkipNode* prev () ;

    SkipNode** fwdNodes;
    SkipNode** bckNodes ;

  private:
    int nodeHeight;
    Key* key;
    Obj* obj;
  };

template <class Key, class Obj>
  SkipNode<Key,Obj>::~SkipNode()
  {
    // if (key != NULL) delete key;
    // if (obj != NULL) delete obj;
    if (fwdNodes != NULL) delete [] fwdNodes;
    if (bckNodes != NULL) delete [] bckNodes ;
  }

template <class Key, class Obj>
  SkipNode<Key,Obj>::SkipNode(Key* k, 
    Obj* o, int h)
  {
    nodeHeight = h;
    key = k;
    obj = o;
    fwdNodes = new SkipNode<Key,Obj>* [h+1];
    bckNodes = new SkipNode<Key,Obj>* [h+1] ;
    for ( int x = 1; x <= nodeHeight; x++ ) {
      fwdNodes[x] = (SkipNode<Key,Obj>*) NULL;
      bckNodes[x] = (SkipNode<Key,Obj>*) NULL;
    }
  }

template <class Key, class Obj>
  SkipNode<Key,Obj>::SkipNode(int h)
  {
    nodeHeight = h;
    key = (Key*) NULL;
    obj = (Obj*) NULL;
    fwdNodes = new SkipNode<Key,Obj>* [h+1];
    bckNodes = new SkipNode<Key,Obj>* [h+1] ;
    for ( int x = 1; x <= nodeHeight; x++ ) {
      fwdNodes[x] = (SkipNode<Key,Obj>*) NULL;
      bckNodes[x] = (SkipNode<Key,Obj>*) NULL;
    }
  }


template <class Key, class Obj>
  Key* SkipNode<Key,Obj>::getKey(void)
  {
    return key;
  }

template <class Key, class Obj>
  Obj* SkipNode<Key,Obj>::getObj(void)
  {
    return obj;
  }

template <class Key, class Obj>
  int SkipNode<Key,Obj>::getHgt(void)
  {
    return nodeHeight;
  }

template <class Key, class Obj>
SkipNode<Key, Obj>* SkipNode<Key, Obj>::next () {
  return fwdNodes[1] ;
}

template <class Key, class Obj>
SkipNode<Key, Obj>* SkipNode<Key, Obj>::prev () {
  return bckNodes[1] ;
}

#endif //SKIP_LIST_NODE

