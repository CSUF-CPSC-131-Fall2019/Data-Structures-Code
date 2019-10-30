#pragma once
#include <iostream>
#include <stdexcept>
#include <algorithm>  // max(), swap()

// This source file has both iterative and recursive implementations for some functions so the two can be studied and compared side
// by side.  To select which version to use (compile and runtime), define either USING_ITERATIVE_FUNCTIONS or
// USING_RECURSIVE_FUNCTIONS macro at compile time with the -DUSING_ITERATIVE_FUNCTIONS (or -DUSING_RECURSIVE_FUNCTIONS) compiler
// switch, or by uncommenting out the line below to change the default from USING_RECURSIVE_FUNCTIONS to USING_ITERATIVE_FUNCTIONS

//#define USING_ITERATIVE_FUNCTIONS
#ifndef USING_ITERATIVE_FUNCTIONS
  #define USING_RECURSIVE_FUNCTIONS
#else
  #undef USING_RECURSIVE_FUNCTIONS
#endif


/*******************************************************************************
**  Binary Search Tree Node Definition
*******************************************************************************/
template <typename Key, typename Value>
struct Node {
  // Constructors
  Node( const Key & key = Key(),  const Value & value = Value() );   // Also serves as the default constructor

  // Public instance attributes
  Key   key_;
  Value value_;

  // Private instance attributes
  Node * left_   = nullptr;
  Node * right_  = nullptr;
  Node * parent_ = nullptr;
};

template <typename Key, typename Value>
std::ostream & operator<<( std::ostream & stream, const Node<Key, Value> & node );


/*******************************************************************************
**  Binary Search Tree Abstract Data Type Definition (Duplicate keys allowed)
*******************************************************************************/
template <typename Key, typename Value>
class BinarySearchTree {
  public:
    BinarySearchTree             () = default;
    BinarySearchTree             ( const BinarySearchTree & original );     // performs a deep copy
    BinarySearchTree & operator= (       BinarySearchTree   rhs      );     // performs a deep copy assignment  NOTE: INTENTIONALLY PASSED BY VALUE (delegates to copy constructor)
   ~BinarySearchTree             ();                                        // performs a deep node destruction

    // Queries
    Value search     ( const Key & key )                       const;       // zyBook 7.4, 7.10:  Returns the value associated with the first node found matching given key. Throws invalid_argument if key not found
    void insert      ( const Key & key, const Value & value );              // zyBook 7.5, 7.9, 7.10:  Inserts a new node populated with key and value in a proper location obeying the BST ordering property.
    void remove      ( const Key & key );                                   // zyBook 7.6, 7.9, 7.10:  Removes the first-found matching node, restructuring the tree to preserve the BST ordering property.
    void printInorder()                                        const;       // zyBook 7.7:  Prints the contents of the tree in ascending sorted order
    int  getHeight   ()                                        const;       // zyBook 7.8:  Returns the height of the tree, or -1 if tree is empty

    void clear();                                                           // Returns the tree to an empty state releasing all nodes


  private:
    Node<Key, Value> * root_ = nullptr;

    // Helper functions
    void clear          ( Node<Key, Value> * node );
    void insertIterative( Node<Key, Value> * node );                                         // zyBook Figure 7.9.1: BSTInsert algorithm for BSTs with nodes containing parent pointers.
    void insertRecursive( Node<Key, Value> * parent, Node<Key, Value> * nodeToInsert );      // zyBook Figure 7.10.2: Recursive BST insertion and removal.
    void remove         ( Node<Key, Value> * node );                                         // zyBook Figure 7.9.3: BSTRemoveKey and BSTRemoveNode algorithms for BSTs with nodes containing parent pointers.  (Figure 7.10.2 identical but passes parent instead of using parent pointer in Node)
    void printInorder   ( Node<Key, Value> * node ) const;                                   // zyBook Figure 7.7.1: BST inorder traversal algorithm.
    int  getHeight      ( Node<Key, Value> * node ) const;                                   // zyBook Figure 7.8.3: BSTGetHeight algorithm.


    Node<Key, Value> * makeCopy       ( Node<Key, Value> * node );                           // Copy constructor helper function

    Node<Key, Value> * searchIterative(                          const Key & key ) const;    // zyBook Figure 7.4.1: BST search algorithm.
    Node<Key, Value> * searchRecursive( Node<Key, Value> * node, const Key & key ) const;    // zyBook Figure 7.10.1: BST recursive search algorithm.

    bool replaceChild( Node<Key, Value> * parent,                                            // zyBook Figure 7.9.2: BSTReplaceChild algorithm.
                       Node<Key, Value> * currentChild,
                       Node<Key, Value> * newChild );

};


/*******************************************************************************
**  BinarySearchTree<Key, Value>  Definitions
*******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
//  Search
////////////////////////////////////////////////////////////////////////////////
template <typename Key, typename Value>
Value BinarySearchTree<Key, Value>::search( const Key  & key ) const
{
  #if defined(USING_ITERATIVE_FUNCTIONS)
    auto node = searchIterative( key );                 // zyBook 7.4.1: BST search algorithm.

  #elif defined(USING_RECURSIVE_FUNCTIONS)
    auto node = searchRecursive( root_, key );          // 7.10.1: BST recursive search algorithm.

  #endif

  if( node == nullptr ) throw std::invalid_argument( "Key not found" );
  return node->value_;
}




//  zyBook 7.4.1: BST search algorithm.
template <typename Key, typename Value>
Node<Key, Value> * BinarySearchTree<Key, Value>::searchIterative( const Key  & key ) const
{
  auto cur = root_;

  while( cur != nullptr )
  {
    if     ( key == cur->key_ )  return cur;      // Found
    else if( key  < cur->key_ )  cur = cur->left_;
    else                         cur = cur->right_;
  }

  return nullptr; // Not found
}




//  zyBook 7.10.1: BST recursive search algorithm.
template <typename Key, typename Value>
Node<Key, Value> * BinarySearchTree<Key, Value>::searchRecursive( Node<Key, Value> * node, const Key & key ) const
{
  if( node != nullptr )
  {
    if     ( key == node->key_ )  return node;
    else if( key  < node->key_ )  return searchRecursive( node->left_,  key );
    else                          return searchRecursive( node->right_, key );
  }

  return nullptr;
}




////////////////////////////////////////////////////////////////////////////////
//  Insert
////////////////////////////////////////////////////////////////////////////////
template <typename Key, typename Value>
void BinarySearchTree<Key, Value>::insert( const Key & key, const Value & value )
{
  auto node = new Node<Key, Value>( key, value );

  #if defined(USING_ITERATIVE_FUNCTIONS)
    insertIterative(        node );                          // Figure 7.9.1: BSTInsert algorithm for BSTs with nodes containing parent pointers.

  #elif defined(USING_RECURSIVE_FUNCTIONS)
    if( root_ == nullptr ) root_ = node;                     // Figure 7.10.2: Recursive BST insertion and removal.
    else                   insertRecursive( root_, node );   // Figure 7.10.2: Recursive BST insertion and removal.

  #endif
}




//  Figure 7.9.1: BSTInsert algorithm for BSTs with nodes containing parent pointers.
template <typename Key, typename Value>
void BinarySearchTree<Key, Value>::insertIterative( Node<Key, Value> * node )
{
  node->left_  = nullptr;                                         // insert as a leaf (added to zyBook algorithm for completeness)
  node->right_ = nullptr;

  if( root_ == nullptr )                                          // Insert first node
  {
    root_         = node;
    node->parent_ = nullptr;

    return;
  }

  auto cur = root_;
  while( cur != nullptr )                                         // Search for insertion point, starting with the root
  {
    if( node->key_ < cur->key_ )
    {
      if( cur->left_ == nullptr )                                 // Found left insertion point
      {
        cur->left_    = node;
        node->parent_ = cur;
        cur           = nullptr;                                  // Stop searching
      }
      else
      {
        cur = cur->left_;                                         // Continue searching left for insertion point
      }
    }

    else   // node->key_ >= cur->key_   (This algorithm allows duplicate keys)
    {
      if( cur->right_ == nullptr )
      {                                                           // Found right insertion point
        cur->right_   = node;
        node->parent_ = cur;
        cur           = nullptr;                                  // Stop searching
      }
      else
      {
        cur = cur->right_;                                        // Continue searching right for insertion point
      }
    }
  }
}




//  zyBook Figure 7.10.2: Recursive BST insertion and removal.  (Assumes parent and nodeToInsert are not null)
template <typename Key, typename Value>
void BinarySearchTree<Key, Value>::insertRecursive( Node<Key, Value> * parent, Node<Key, Value> * nodeToInsert )
{
  if( nodeToInsert->key_ < parent->key_ )
  {
    if( parent->left_ == nullptr )
    {
      parent->left_         = nodeToInsert;
      nodeToInsert->parent_ = parent;                             // Not in zyBook algorithm, but needed when tree node contains parent pointer
    }
    else insertRecursive( parent->left_, nodeToInsert );
  }
  else
  {
    if( parent->right_ == nullptr )
    {
      parent->right_        = nodeToInsert;
      nodeToInsert->parent_ = parent;                              // Not in zyBook algorithm, but needed when tree node contains parent pointer
    }
    else insertRecursive( parent->right_, nodeToInsert );
  }
}




////////////////////////////////////////////////////////////////////////////////
//  Remove
////////////////////////////////////////////////////////////////////////////////
//  zyBook Figure 7.9.3: BSTRemoveKey and BSTRemoveNode algorithms for BSTs with nodes containing parent pointers.
template <typename Key, typename Value>
void BinarySearchTree<Key, Value>::remove( const Key & key )
{
  #if defined(USING_ITERATIVE_FUNCTIONS)
    auto node = searchIterative( key );

  #elif defined(USING_RECURSIVE_FUNCTIONS)
    auto node = searchRecursive( root_, key );

  #endif

  remove( node );
}




//  zyBook Figure 7.9.3: BSTRemoveKey and BSTRemoveNode algorithms for BSTs with nodes containing parent pointers.
template <typename Key, typename Value>
void BinarySearchTree<Key, Value>::remove( Node<Key, Value> * node )
{
  if( node == nullptr ) return;

  // Case 1: Internal node with 2 children
  if ( node->left_ != nullptr  &&  node->right_ != nullptr)
  {
    // Find successor (leftmost child of right subtree)
    auto succNode = node->right_;
    while( succNode->left_ != nullptr ) succNode = succNode->left_;

    // Copy value/data from succNode to node
    node->key_   = succNode->key_;
    node->value_ = succNode->value_;

    // Recursively remove succNode
    remove( succNode );
  }


  else
  {
    // Case 2: Root node (with 1 or 0 children)
    if (node == root_)
    {
      if( node->left_ != nullptr )  root_ = node->left_;
      else                          root_ = node->right_;

      // Make sure the new root, if non-null, has a null parent
      if( root_ != nullptr ) root_->parent_ = nullptr;
    }



    // Case 3: Internal with left child only
    else if( node->left_ != nullptr )   replaceChild( node->parent_, node, node->left_ );

    // Case 4: Internal with right child only OR leaf
    else                                replaceChild( node->parent_, node, node->right_ );

    delete node;  // Not in zyBook algorithm, but needed to prevent memory leak
  }
}




//  zyBook Figure 7.9.2: BSTReplaceChild algorithm.
template <typename Key, typename Value>
bool BinarySearchTree<Key, Value>::replaceChild( Node<Key, Value> * parent,
                                                 Node<Key, Value> * currentChild,
                                                 Node<Key, Value> * newChild )
{
  if( parent->left_ != currentChild  &&  parent->right_ != currentChild )   return false;

  if( parent->left_ == currentChild )  parent->left_  = newChild;
  else                                 parent->right_ = newChild;

  if( newChild != nullptr ) newChild->parent_ = parent;

  return true;
}




////////////////////////////////////////////////////////////////////////////////
//  Print
////////////////////////////////////////////////////////////////////////////////
template <typename Key, typename Value>
void BinarySearchTree<Key, Value>::printInorder() const
{
  printInorder( root_ );
}




//  zyBook Figure 7.7.1: BST inorder traversal algorithm.
template <typename Key, typename Value>
void BinarySearchTree<Key, Value>::printInorder( Node<Key, Value> * node ) const
{
  if( node == nullptr ) return;

  printInorder( node->left_ );
  std::cout << *node;
  printInorder( node->right_ );
}




////////////////////////////////////////////////////////////////////////////////
//  Height
////////////////////////////////////////////////////////////////////////////////
template <typename Key, typename Value>
int BinarySearchTree<Key, Value>::getHeight() const
{
  return getHeight( root_ );
}




//  zyBook Figure 7.8.3: BSTGetHeight algorithm.
template <typename Key, typename Value>
int BinarySearchTree<Key, Value>::getHeight( Node<Key, Value> * node ) const
{
  if( node == nullptr ) return -1;

  auto leftHeight  = getHeight( node->left_  );
  auto rightHeight = getHeight( node->right_ );

  return 1 + std::max( leftHeight, rightHeight );
}




////////////////////////////////////////////////////////////////////////////////
//   Constructors, destructor, assignments
////////////////////////////////////////////////////////////////////////////////
template <typename Key, typename Value>
BinarySearchTree<Key, Value>::BinarySearchTree( const BinarySearchTree & original )
{ root_ = makeCopy( original.root_ ); }




template <typename Key, typename Value>
Node<Key, Value> * BinarySearchTree<Key, Value>::makeCopy( Node<Key, Value> * originalNode )
{
  if( originalNode == nullptr ) return nullptr;

  auto node    = new Node<Key, Value>( originalNode->key_, originalNode->value_ );
  node->left_  = makeCopy( originalNode->left_ );
  node->right_ = makeCopy( originalNode->right_ );

  if( node->left_  != nullptr ) node->left_ ->parent_ = node;
  if( node->right_ != nullptr ) node->right_->parent_ = node;

  return node;
}




// Passing by value delegates copying the tree to the copy constructor, keeping the "copy" knowledge
// in one place.  (Copy and swap idiom)
template <typename Key, typename Value>
BinarySearchTree<Key, Value> & BinarySearchTree<Key, Value>::operator=( BinarySearchTree rhs )
{
  auto temp = rhs.root_;
  rhs.root_ = root_;
  root_     = temp;

  return *this;
}




template <typename Key, typename Value>
BinarySearchTree<Key, Value>::~BinarySearchTree()
{ clear(); }




template <typename Key, typename Value>
void BinarySearchTree<Key, Value>::clear()
{
  clear( root_ );
  root_ = nullptr;
}




template <typename Key, typename Value>
void BinarySearchTree<Key, Value>::clear( Node<Key, Value> * node )
{
  if( node == nullptr ) return;

  clear( node->left_ );
  clear( node->right_ );

  delete node;
}




/*******************************************************************************
**  Node<Key, Value>  Definitions
*******************************************************************************/
template <typename Key, typename Value>
Node<Key, Value>::Node( const Key & key, const Value & value )
  : key_( key ), value_( value )
{}




template <typename Key, typename Value>
std::ostream & operator<<( std::ostream & stream, const Node<Key, Value> & node )
{
  stream << "Key: \"" << node.key_ << "\",  Value: \"" << node.value_ << "\"\n";
  return stream;
}
