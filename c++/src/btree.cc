#include <btree.h>
#include <memory>
#include <iostream>
#include <iterator>
#include <iomanip> //indentation
#include <math.h>
#include <vector> 


template<class TK, class TV, class Tcomp>
void BTree<TK,TV,Tcomp>::insert( std::pair<TK,TV> p ){ 
  if( root == nullptr){
    // invalid.reset(new Invalid_BNode{p});
    root.reset(new BNode{p});
    //std::cout << "invalid node setting " << invalid->pair.first <<std::endl;
  }
  else
    this->root->insert_node(p); 
}

// insert node
template<class TK, class TV, class Tcomp>
void BTree<TK,TV,Tcomp>::BNode::insert_node(std::pair<TK,TV> p){
  // key is already present
  if (!comparison(this->pair.first, p.first) && !comparison(p.first,this->pair.first) ){
    // if( this->pair.first == p.first ){
    this->pair.second = p.second;
    // std::cout << "already present" << std::endl;
  }
  else if (comparison(p.first,this->pair.first) ){
    //else if( this->pair.first > p.first ){
    if( this->left == nullptr ){
      this->left.reset(new BNode{p, this});
      //this->left->next = this; //parent setting
      return;
    }
    else
      this->left->insert_node(p);
  }    
  else{
    if( this->right == nullptr ){
      this->right.reset(new BNode{p, this->next});
      //this->right->next = this; //parent setting
      return;
    }
    else
      this->right->insert_node(p);

  }
}


// BTree::print
template<class TK, class TV, class Tcomp>
void BTree<TK,TV,Tcomp>::print() {
  using ConstIterator = BTree<TK,TV,Tcomp>::ConstIterator;
  ConstIterator it = this->cbegin(), end = this->cend();
  // std::cout << "print" << std::endl;
  for( ; it != end; ++it )
  	std::cout << (*it).first << " " << (*it).second << std::endl; 
}

// BTree::clear()
template<class TK, class TV, class Tcomp>
void BTree<TK,TV,Tcomp>::clear(){
  if (root==nullptr){
  	#ifdef DEBUG
        std::cout<<"BTree::clear() : Tree is empty"<<"\n";
    	#endif
  	return;
  }
  root->clear_node();
  root.reset(nullptr);
}


// BNode::clear_node()
template <class TK, class TV, class Tcomp>
void BTree<TK,TV,Tcomp>::BNode::clear_node(){

	if( this->left != nullptr ){
		this->left->clear_node();
		}
	if( this->right != nullptr ){
		this->right->clear_node();
		}
	
	this->left.reset(nullptr);
	this->right.reset(nullptr);
}

// ~BTree()
template<class TK, class TV, class Tcomp>
BTree<TK,TV,Tcomp>::~BTree(){
	#ifdef DEBUG
	std::cout << "~BTree()" << std::endl;
	#endif
  	this->clear();
}



//BALANCE FUNCTIONS
template<class TK, class TV, class Tcomp>
void BTree<TK,TV,Tcomp>::balance(){
#ifdef DEBUG
  std::cout << "BTree()::balance()" << std::endl;
#endif
  std::vector<std::pair<TK,TV>> vec;

  using Iterator = BTree<TK,TV,Tcomp>::Iterator;
  Iterator it = this->begin(), end = this->end();
  for( ; it != end; ++it ){
    vec.push_back(*it);
    // std::cout<< "blancing " <<(*it).first <<std::endl;
  }
  
  clear();
  
  int size = vec.size();

  built_tree(vec,0,size-1);
}

template<class TK, class TV, class Tcomp>
void BTree<TK,TV,Tcomp>::built_tree( std::vector<std::pair<TK,TV>> &vec, int start, int end){
  if (start>end) return;
  int half = (start+end)/2;
  // std::cout<<"start "<<start<<", end "<<end<<", half "<<half<<std::endl;
  
  std::pair<TK,TV> p= vec[half];
  
  insert(p);
  
  built_tree(vec, start, half-1);
  built_tree(vec, half+1, end);
  //n->right->built_tree(vec, half+1, end);

}



template<class TK, class TV, class Tcomp>
bool BTree<TK,TV,Tcomp>::isbalanced(){
  if (root!=nullptr){
#ifdef DEBUG
    std::cout << "isbalanced: root!= nullptr" << std::endl;
#endif
    return (this->root->isbalanced_node());
  }
  else {
    std::cout<<"blanced tree"<<std::endl;
    return 0;
  }
}

template<class TK, class TV, class Tcomp>
int BTree<TK,TV,Tcomp>::BNode::isbalanced_node(){

  if (this != nullptr){
    if (this->left != nullptr && this->right != nullptr){
      int diff = this->left->isbalanced_node() - this->right->isbalanced_node() ;
#ifdef DEBUG
      std::cout<<"diff "<<diff<<std::endl;
#endif
      return diff;
    }

    if ( (this->left==nullptr && this->right!=nullptr) || (this->left!=nullptr && this->right==nullptr)) {
#ifdef DEBUG
      std::cout<<"unblanced tree"<<std::endl;
#endif
      return 1;
    }
    else if (this->left==nullptr && this->right==nullptr) {
#ifdef DEBUG
      std::cout<<"blanced tree"<<std::endl;
#endif
      return 0;
    }
  }
#ifdef DEBUG
  std::cout<<"blanced tree"<<std::endl;
#endif
  return 0;
 
}

template<class TK, class TV, class Tcomp>
void BTree<TK,TV,Tcomp>::erase(TK key){
 if (root!=nullptr)
   this->root->find_node(key);
}

template<class TK, class TV, class Tcomp>
void BTree<TK,TV,Tcomp>::BNode::find_node(TK key){
  if (this!=nullptr){
    //if (this->pair.first==key) this->erase_node();
    if ( !comparison(this->pair.first, key) && !comparison(key,this->pair.first)  )  this->erase_node();
    //else if (this->pair.first>key) this->left->find_node(key);
    else if (comparison(key,this->pair.first) ) this->left->find_node(key);
    else this->right->find_node(key);
  }
  return;
}

template<class TK, class TV, class Tcomp>
void BTree<TK,TV,Tcomp>::BNode::erase_node(){

    //node has no childs
   if (this->left==nullptr && this->right == nullptr){
    std::cout<<"no childs"<<std::endl;
    //if(this->next!=nullptr)
    //std::cout<<"printing next "<<this->next->pair.first<<" "<<this->next->pair.second<<std::endl;
    }
   
   //node has only left child
   if (this->left!= nullptr && this->right==nullptr){
     this->pair.first=this->left->pair.first;
     this->pair.second=this->left->pair.second;
      if (this->left->left==nullptr && this->left->right==nullptr) this->left.reset(nullptr);
      else this->left->erase_node();
    }
    //node has only right child
    if (this->left== nullptr && this->right!=nullptr){
      this->pair.first=this->right->pair.first;
      this->pair.second=this->right->pair.second;
      this->right->erase_node();
      if (this->right->left==nullptr && this->right->right==nullptr) this->right.reset(nullptr);
      else this->right->erase_node();
    }
    //node has both childs
    if (this->left!= nullptr && this->right!=nullptr){
      this->pair.first=this->right->pair.first;
      this->pair.second=this->right->pair.second;
      //std::cout<<"here "<<this->pair.first<<" "<<this->pair.second<<std::endl;
      if (this->right->left==nullptr && this->right->right==nullptr) this->right.reset(nullptr);
      else this->right->erase_node();
      // this->right->erase_node();
    }

  return;

}


template<class TK, class TV, class Tcomp>
void BTree<TK,TV,Tcomp>::diagram(){
  if (root!=nullptr)
  this->root->diagram(20);  
}

template<class TK, class TV, class Tcomp>
void BTree<TK,TV,Tcomp>::BNode::diagram(int indent){
   if(this != nullptr) {
    if (indent) {
      std::cout << std::setw(indent) << ' ';
      std::cout<< this->pair.first<<std::endl;
    }
    if (this->left) {
      this->left->diagram(indent - 2);
    }
    if (this->right) {
      this->right->diagram(indent + 2);
    }
   }
}


// explicit template
template class BTree<int,int, std::less<int>>;