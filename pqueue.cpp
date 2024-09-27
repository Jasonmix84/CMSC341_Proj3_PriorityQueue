// CMSC 341 - Fall 2023 - Project 3
#include "pqueue.h"
PQueue::PQueue(prifn_t priFn, HEAPTYPE heapType, STRUCTURE structure)
: m_priorFunc(priFn), m_heapType(heapType), m_structure(structure) {
  //basic constructor
  m_heap = nullptr;
  m_size = 0;
}

PQueue::~PQueue()
{
  //clear handles deallocation
  clear();
}
void PQueue::clear() {
  recursiveClear(m_heap);
  m_heap = nullptr;
  m_size = 0;

}
PQueue::PQueue(const PQueue& rhs)
{
  //copy constructor
  //rhs is a premade PQ all i do is check that if it is empty we make an ampty PQUEUE also
  if (rhs.m_heap == nullptr || rhs.m_size == 0){
    //keep the copy constructor as an empty object
    return;
  }
  else{ 
    //call recursive Tree constructor
    m_heap = reconstructTree(rhs.m_heap);
    
    m_size = rhs.m_size;
    m_priorFunc = rhs.m_priorFunc;
    m_heapType = rhs.m_heapType;
    m_structure = rhs.m_structure;
  }
}

PQueue& PQueue::operator=(const PQueue& rhs) {
  //prevent self assignment
  if (this == &rhs){
    return *this;
  }

  //clear data in m_heap lhs
  //sets everything to zero also
  clear();
  m_heap = nullptr;
  m_size = 0;


  //make a copy of rhs

  m_heap = reconstructTree(rhs.m_heap);
  
  m_size = rhs.m_size;
  m_priorFunc = rhs.m_priorFunc;
  m_heapType = rhs.m_heapType;
  m_structure = rhs.m_structure;
  return *this;
}
void PQueue::mergeWithQueue(PQueue& rhs) {
  //there are 4 possible combos:
  //min leftist heap
  //max leftist heap
  //min skew heap
  //max skew heap
  //the two merging must have the same combo
  if (m_heapType !=  rhs.getHeapType() || m_structure != rhs.getStructure() || m_priorFunc != rhs.getPriorityFn()){
    //throw domain_error
    throw domain_error("domain error");
  }
  else{ 
    //calling merge on the same object
    if (this == &rhs){
      return;
    }
    
    if (m_structure == SKEW){
      if (m_heapType == MINHEAP){
        //skew minheap
        m_heap = skewMergeMin(m_heap, rhs.m_heap);
      }
      else{
        //skew maxheap
        m_heap = skewMergeMax(m_heap, rhs.m_heap);
      }
    }
    else{
      if (m_heapType == MINHEAP){
        //leftist minheap
        m_heap = leftistMergeMin(m_heap, rhs.m_heap);
      }
      else{
        //leftist maxheap
        m_heap = leftistMergeMax(m_heap, rhs.m_heap);
      }
    }
    m_size += rhs.m_size;
    rhs.m_heap = nullptr;
    rhs.m_size = 0;
    rhs.clear();//may cause an error by adding this
  }
}

void PQueue::insertPatient(const Patient& patient) {
  //create new node with patient information
  Node* newN = new Node(patient);
  if (m_heap == nullptr){
    //inserting the first node to an empty PQueue
    m_heap = newN;
    m_size++;
    newN = nullptr;
    delete newN;
  }
  else{
    //create a new PQueue with a single node to merge and add to existing
    //must be of the same types as current nodes
    PQueue newPQ(m_priorFunc, m_heapType, m_structure); //basically making NEW pqueue
    newPQ.m_heap = newN;
    newPQ.m_size++;
    mergeWithQueue(newPQ);
    newN = nullptr;
    //deallocation
    delete newN;
  }
}

int PQueue::numPatients() const
{
  //return size
  return m_size;
}

prifn_t PQueue::getPriorityFn() const {
  //return priority
  return m_priorFunc;
}
Patient PQueue::getNextPatient() {
    //This function extracts (removes the node) and returns 
    //the highest priority patient from the queue. It must maintain the min-heap or max-heap property.
    // The function throws an out_of_range exception if the queue is empty when the function is called.
    if (m_size == 0 && m_heap == nullptr){
      //error case where getNextPatient is called on an empty heap
      throw out_of_range("error out of range");
    }
    else{
      if (m_heap->m_left == nullptr && m_heap->m_right == nullptr){
        //case where there is only a single node in the heap
        m_size--;
        Patient temp = m_heap->getPatient();
        return temp; 
      }
      Node* deathPtr = m_heap;
      if (m_heap->m_left != nullptr && m_heap->m_right == nullptr){
        //heap has only left children
        //set root of the heap to that left child
        m_heap = m_heap->m_left;
        //set the left child of the orignial root to null since it is now seperated
        deathPtr->m_left = nullptr;

      }
      else if (m_heap->m_left == nullptr && m_heap->m_right != nullptr){
        //heap has only right children
        //set root of the heap to that right child
        m_heap = m_heap->m_right;
        //set the right child of the orignial root to null since it is now seperated
        deathPtr->m_right = nullptr;
      }
      else{
        //pick higher priority child
        //use this simple helper function to get the child with highest priority
        m_heap = pickChild(m_heap, m_heapType);
        //set left or right that was detached to nullptr
        if (deathPtr->m_left == m_heap){
          deathPtr->m_left = nullptr;
          //make a new PQ
          PQueue PQue(m_priorFunc, m_heapType, m_structure);
          PQue.m_heap = deathPtr->m_right;
          //PQue.m_size = countHeap(PQue.m_heap);
          mergeWithQueue(PQue);
          
        }
        else{
          deathPtr->m_right = nullptr;
          //make a new PQ
          PQueue PQue(m_priorFunc, m_heapType, m_structure);
          PQue.m_heap = deathPtr->m_left;
          //PQue.m_size = countHeap(PQue.m_heap);
          mergeWithQueue(PQue);
          
        }
      }
      m_size--;
      Patient temp = deathPtr->getPatient();
      delete deathPtr;
      return temp;
    }
    
}

void PQueue::setPriorityFn(prifn_t priFn, HEAPTYPE heapType) {
  // Set a new priority function.  
  //Must rebuild the heap!!! so basically its like a reconstruct call
  //can crate a new min or max heap

  if (priFn == m_priorFunc && heapType == m_heapType){
    //if its the same dont do anything
    return;
  }
  else{
    //create a new heap
    PQueue PQue(priFn, heapType, m_structure);
    //insert into that heap with new priority and new heaptype
    recurseStruct(m_heap, priFn, heapType, m_structure, PQue);
    //clear out current heap
    clear();
    //set everything to the new heap
    m_heap = PQue.m_heap;
    m_size = PQue.m_size;
    m_priorFunc = PQue.m_priorFunc;
    m_heapType = PQue.m_heapType;
    m_structure = PQue.m_structure;
    //deallocate the heap
    PQue.m_heap = nullptr;
    PQue.m_size = 0;
    PQue.clear();
  }


}

void PQueue::setStructure(STRUCTURE structure){
  // Set a new data structure (skew/leftist). 
  //Must rebuild the heap!!! another reconstruct type function
  if (m_structure == structure){
    //if its the same dont do anything
    return;
  }
  else{
    //create a new heap
    PQueue PQue(m_priorFunc, m_heapType, structure);
    //insert into that heap with new priority and new heaptype
    recurseStruct(m_heap, m_priorFunc, m_heapType, structure, PQue);
    //clear out current heap
    clear();
    //set everything to the new heap
    m_heap = PQue.m_heap;
    m_size = PQue.m_size;
    m_priorFunc = PQue.m_priorFunc;
    m_heapType = PQue.m_heapType;
    m_structure = PQue.m_structure;
    //deallocate the heap
    PQue.m_heap = nullptr;
    PQue.m_size = 0;
    PQue.clear();
  }
}

STRUCTURE PQueue::getStructure() const {
  //return structure
  return m_structure;
}

HEAPTYPE PQueue::getHeapType() const {
  //return heaptype
  return m_heapType;
}

void PQueue::printPatientQueue() const {
  //print using preorder traversal 
  recursivePrint(m_heap);
}

void PQueue::dump() const {
  if (m_size == 0) {
    cout << "Empty heap.\n" ;
  } else {
    dump(m_heap);
  }
  cout << endl;
}
void PQueue::dump(Node *pos) const {
  if ( pos != nullptr ) {
    cout << "(";
    dump(pos->m_left);
    if (m_structure == SKEW)
        cout << m_priorFunc(pos->m_patient) << ":" << pos->m_patient.getPatient();
    else
        cout << m_priorFunc(pos->m_patient) << ":" << pos->m_patient.getPatient() << ":" << pos->m_npl;
    dump(pos->m_right);
    cout << ")";
  }
}

ostream& operator<<(ostream& sout, const Patient& patient) {
  sout  << patient.getPatient() 
        << ", temperature: " << patient.getTemperature()
        << ", oxygen: " << patient.getOxygen()
        << ", respiratory: " << patient.getRR()
        << ", blood pressure: " << patient.getBP()
        << ", nurse opinion: " << patient.getOpinion();
  return sout;
}
ostream& operator<<(ostream& sout, const Node& node) {
  sout << node.getPatient();
  return sout;
}


/////////////////////////Helper Function Implementations////////////////////////////////

//testing this first
Node* PQueue::skewMergeMin(Node* lhsH, Node* rhsH){
  //this is for a skew min heap
  if (lhsH == nullptr){
    return rhsH;
  }
  else if (rhsH == nullptr){
    return lhsH;
  }
  else if (m_priorFunc(lhsH->m_patient) <= m_priorFunc(rhsH->m_patient)){
    //lhsH has a higher priority for a minheap
    swap(lhsH->m_left, lhsH->m_right, lhsH);
    //swapped the two parameters of skewMerge
    lhsH->m_left = skewMergeMin(lhsH->m_left, rhsH);
    return lhsH;
  }
  else{
    //rhsH has a higher priority for a minheap
    swap(rhsH->m_left, rhsH->m_right, rhsH);
    rhsH->m_left = skewMergeMin(lhsH, rhsH->m_left);
    return rhsH;
  }
  //error case
  return nullptr;
}

Node* PQueue::skewMergeMax(Node* lhsH, Node* rhsH){
  //this is for a skew max heap
  if (lhsH == nullptr){
    return rhsH;
  }
  else if (rhsH == nullptr){
    return lhsH;
  }
  else if (m_priorFunc(lhsH->m_patient) >= m_priorFunc(rhsH->m_patient)){
    //lhsH has a higher priority
    swap(lhsH->m_left, lhsH->m_right, lhsH);
    //swapped the two parameters of skewMerge
    lhsH->m_left = skewMergeMax(lhsH->m_left, rhsH);
    return lhsH;
  }
  else{
    //rhsH has a higher priority
    swap(rhsH->m_left, rhsH->m_right, rhsH);
    rhsH->m_left = skewMergeMax(lhsH, rhsH->m_left);
    return rhsH;
  }

  return nullptr;
}

Node* PQueue::leftistMergeMin(Node* lhsH, Node* rhsH){
  if (lhsH == nullptr){
    return rhsH;
  }
  else if (rhsH == nullptr){
    return lhsH;
  }
  else if (m_priorFunc(lhsH->m_patient) <= m_priorFunc(rhsH->m_patient)){
    //lhsH has a higher priority
    return nplMerge(lhsH, rhsH, MINHEAP);
  }
  else{
    //rhsH has a higher priority
    return nplMerge(rhsH, lhsH, MINHEAP);
  }

  return nullptr;
}

Node* PQueue::leftistMergeMax(Node* lhsH, Node* rhsH){
  if (lhsH == nullptr){
    return rhsH;
  }
  else if (rhsH == nullptr){
    return lhsH;
  }
  else if (m_priorFunc(lhsH->m_patient) >= m_priorFunc(rhsH->m_patient)){
    //lhsH has a higher priority
    return nplMerge(lhsH, rhsH, MAXHEAP);
  }
  else{
    //rhsH has a higher priority
    return nplMerge(rhsH, lhsH, MAXHEAP);
  }

  return nullptr;
}

Node* PQueue::nplMerge(Node* lhsH, Node* rhsH, HEAPTYPE type){
  if (lhsH->m_left == nullptr){
    //always want to make left side heavier
    lhsH->m_left = rhsH;
  }
  else{
    //0 is equivalent to mintype
    if (type == 0){
      lhsH->m_right = leftistMergeMin(lhsH->m_right, rhsH);
    }
    else{
      lhsH->m_right = leftistMergeMax(lhsH->m_right, rhsH);
    }
    if (lhsH->m_left->getNPL() < lhsH->m_right->getNPL()){
      //we swap children if violates leftNPL> rightNPL
      swap(lhsH->m_left, lhsH->m_right, lhsH);
    }
    //update npl
    lhsH->setNPL(lhsH->m_right->getNPL() + 1);
  }
  return lhsH;
}

void PQueue::swap(Node* lhs, Node* rhs, Node* parent){
  //swap the two children
  parent->m_left = rhs;
  parent->m_right = lhs;
}

void PQueue::recursivePrint(Node* root) const{
  //print in pre order traversal
  if (root == nullptr){
    return;
  }
  cout << "[" << m_priorFunc(root->m_patient) << "] ";
  cout << *root << endl;

  recursivePrint(root->m_left);
  recursivePrint(root->m_right);
}

void PQueue::recursiveClear(Node* root){
  //delete in post order
  if (root == nullptr){
    return;
  }
  recursiveClear(root->m_left);
  recursiveClear(root->m_right);
  delete root;
  root = nullptr;
}

void PQueue::recurseStruct(Node* root, prifn_t prifunc, HEAPTYPE hType, STRUCTURE structure, PQueue& newPQ){
  //using pre order
  if (root == nullptr){
    return;
  }
  //create a new patient
  Patient* copyPatient = new Patient();
  //use overloaded assignment opperator
  *copyPatient = root->m_patient;
  //call insert on each copy of patient
  newPQ.insertPatient(*copyPatient);
  //deallocate
  delete copyPatient;

  recurseStruct(root->m_left, prifunc, hType, structure, newPQ);
  recurseStruct(root->m_right, prifunc, hType, structure, newPQ);
}

Node* PQueue::pickChild(Node* root, HEAPTYPE hType){
  //picking child with higher priority bases on type i.e. minheap or maxheap
  if (hType == MINHEAP){
    return (m_priorFunc(root->m_left->m_patient) < m_priorFunc(root->m_right->m_patient)) ? root->m_left : root->m_right;
  }
  else{
    return (m_priorFunc(root->m_left->m_patient) > m_priorFunc(root->m_right->m_patient)) ? root->m_left : root->m_right;
  }


}

Node* PQueue::reconstructTree(Node* root){
  //recurse tree making copynodes that point to each other
  //ultimately recreates a tree
  
  if (root == nullptr){
    //reaches the end of a subtree
    return nullptr;
  }

  //this makes a new patient and a new Node that holds a copy of the patient data
  Node* copyNode = new Node(root->m_patient);
  //incase we copy leftist tree
  copyNode->m_npl = root->m_npl;

  //recurse
  copyNode->m_left = reconstructTree(root->m_left);

  copyNode->m_right = reconstructTree(root->m_right);

  //return new root node
  return copyNode;

}
