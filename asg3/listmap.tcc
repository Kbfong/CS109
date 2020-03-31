// $Id: listmap.tcc,v 1.12 2019-02-21 17:28:55-08 - - $

#include "listmap.h"
#include "debug.h"


/*

This program was completed using pair programming.
Partner:  Kody Fong (kbfong@ucsc.edu)
Partner:  Andy Huang (ahuang44@ucsc.edu)
*/

//
/////////////////////////////////////////////////////////////////
// Operations on listmap::node.
/////////////////////////////////////////////////////////////////
//

//
// listmap::node::node (link*, link*, const value_type&)
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::node::node (node* next_, node* prev_,
                                     const value_type& value_):
            link (next_, prev_  ), value (value_) {
}

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::~listmap() {
   DEBUGF ('l', reinterpret_cast<const void*> (this));
   while(not empty()) 
   {
      erase(begin()); //frees map from everything 
   }
}


//
// iterator listmap::insert (const value_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::insert (const value_type& pair) {
    listmap<Key,Value,Less>::iterator temp;
    //check if listmap is empty
    if(empty()){
        //if so, make new node
        node* newNode = new node(anchor(), anchor(), pair);
        //assign anchors' next and prev to it making it the first entry.
        anchor()->next=newNode;
        anchor()->prev=newNode;
        //assign temp to newNode to be returned at end
        temp=newNode;
    }
    //not empty
    else{
        //if its not empty we need to see if it is already in the listmap
        temp=find(pair.first);
        //if temp is end, then it was not found 
        if(temp!=end()){
            //if found, we replace the value
            temp->second=pair.second;
        }
        //not found
        else{
            //start at the top
            temp=begin();
            //while not at the end of listmap or until pair.fist is less than temp.first
            while(temp!=end() and less(temp->first,pair.first)){
                ++temp;
            }
            //creates new node with next=insert position and prev=insert position's prev
            node* newNode = new node(temp.where,temp.where->prev,pair);
            //repair newNode's next's prev to be new node.
            newNode->next->prev=newNode;
            //repair newNode's prev's next to be new node.
            newNode->prev->next=newNode;
            //after these 2 repairs newNode sits in between with correct pointers.
            //set temp to newNode bc we return an iterator pointing at the inserted item
            temp=newNode;
        }
    }
    
    
    
   DEBUGF ('l', &pair << "->" << pair);
   return temp;
}

//
// listmap::find(const key_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::find (const key_type& that) {
    // iterator 
    listmap<Key,Value,Less>::iterator it = begin();
    // loop until iterator hits the end or if it finds the value
    while(it != end() && it->first !=that)
    {
        ++it;
    }
   DEBUGF ('l', that);
   //will return either end() or what we are looking for
   return it;
}

//
// iterator listmap::erase (iterator position)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::erase (iterator position) {
    // if only one node in the listmap
    if(position.where->next == position.where){
        delete position.where;
        
    }
    else{
        //temp holds node after given position
        listmap<Key,Value,Less>::iterator temp = position.where->next;
        //reassign target's prev's next to target's next 
        position.where->prev->next=position.where->next;
        //target's next's prev to target's previous
        position.where->next->prev=position.where->prev;
        delete position.where;
        //returns the next thing after the deleted
        return temp;
    }
   DEBUGF ('l', &*position);
   return iterator();
}


//
/////////////////////////////////////////////////////////////////
// Operations on listmap::iterator.
/////////////////////////////////////////////////////////////////
//

//
// listmap::value_type& listmap::iterator::operator*()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type&
listmap<Key,Value,Less>::iterator::operator*() {
   DEBUGF ('l', where);
   return where->value;
}

//
// listmap::value_type* listmap::iterator::operator->()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type*
listmap<Key,Value,Less>::iterator::operator->() {
   DEBUGF ('l', where);
   return &(where->value);
}

//
// listmap::iterator& listmap::iterator::operator++()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator++() {
   DEBUGF ('l', where);
   where = where->next;
   return *this;
}

//
// listmap::iterator& listmap::iterator::operator--()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator--() {
   DEBUGF ('l', where);
   where = where->prev;
   return *this;
}


//
// bool listmap::iterator::operator== (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator==
            (const iterator& that) const {
   return this->where == that.where;
}

//
// bool listmap::iterator::operator!= (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator!=
            (const iterator& that) const {
   return this->where != that.where;
}

