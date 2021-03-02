// Project identifier: AD48FB4835AF347EB0CA8009E24C3B13F8519882

#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <utility>

// A specialized version of the 'priority_queue' ADT implemented as a pairing heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
        public:
            explicit Node(const TYPE &val)
        : elt{ val }, child{ nullptr }, sibling{ nullptr }, parent { nullptr }
            {}

            // Description: Allows access to the element at that Node's position.
			// There are two versions, getElt() and a dereference operator, use
			// whichever one seems more natural to you.
            // Runtime: O(1) - this has been provided for you.
            const TYPE &getElt() const { return elt; }
            const TYPE &operator*() const { return elt; }

            // The following line allows you to access any private data members of this
            // Node class from within the PairingPQ class. (ie: myNode.elt is a legal
            // statement in PairingPQ's add_node() function).
            friend PairingPQ;

        private:
            TYPE elt;
            Node *child;
            Node *sibling;
            Node* parent; // look at meld, constructors
    }; // Node


    // Description: Construct an empty priority_queue with an optional comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
    BaseClass{ comp }, rootNode { nullptr }, numNodes { 0 } {
    } // PairingPQ()


    // Description: Construct a priority_queue out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
    BaseClass{ comp }, rootNode { nullptr }, numNodes { 0 } {
        for (InputIterator i = start; i < end; ++i) {
            push(*i);
        }
    } // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ &other) :
    BaseClass{ other.compare }, rootNode { nullptr }, numNodes { 0 } {
        std::deque<Node*> copydeck;
        copydeck.push_back(other.rootNode);
        
        while (!copydeck.empty()) {
            if (copydeck.front()->child != nullptr) {
                copydeck.push_back(copydeck.front()->child);
            }
            if (copydeck.front()->sibling != nullptr) {
                copydeck.push_back(copydeck.front()->sibling);
            }
            push(copydeck.front()->elt);
            copydeck.pop_front();
        }
    } // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    PairingPQ &operator=(const PairingPQ &rhs) {
        PairingPQ temp(rhs);
    
        std::swap(rootNode, temp.rootNode);
        std::swap(numNodes, temp.numNodes);

        return *this;
    } // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
        if (rootNode != nullptr) {
            std::deque<Node*> copydeck;
            copydeck.push_back(rootNode);
            
            while (!copydeck.empty()) {
                if (copydeck.front()->child != nullptr) {
                    copydeck.push_back(copydeck.front()->child);
                }
                if (copydeck.front()->sibling != nullptr) {
                    copydeck.push_back(copydeck.front()->sibling);
                }
                delete copydeck.front();
                copydeck.pop_front();
            }
        }
    } // ~PairingPQ()


    // Description: Assumes that all elements inside the priority_queue are out of order and
    //              'rebuilds' the priority_queue by fixing the priority_queue invariant.
    //              You CANNOT delete 'old' nodes and create new ones!
    // Runtime: O(n)
    virtual void updatePriorities() {
        std::deque<Node*> copydeck;
        copydeck.push_back(rootNode);
        rootNode = nullptr;
        
        while (!copydeck.empty()) {
            Node* temp = copydeck.front();
            copydeck.pop_front();
            if (temp->child != nullptr) {
                copydeck.push_back(temp->child);
            }
            if (temp->sibling != nullptr) {
                copydeck.push_back(temp->sibling);
            }
            
            temp->sibling = nullptr;
            temp->child = nullptr;
            temp->parent = nullptr;
            rootNode = meld(rootNode, temp);
        }
        
    } // updatePriorities()


    // Description: Add a new element to the priority_queue. This is already done.
    //              You should implement push functionality entirely in the addNode()
    //              function, and this function calls addNode().
    // Runtime: O(1)
    virtual void push(const TYPE &val) {
        addNode(val);
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element from
    //              the priority_queue.
    // Note: We will not run tests on your code that would require it to pop an
    // element when the priority_queue is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
        if (numNodes == 1) {
            delete rootNode;
            rootNode = nullptr;
            --numNodes;
        }
        else if (numNodes == 2) {
            Node* temp = rootNode;
            rootNode = rootNode->child;
            delete temp;
            --numNodes;
        }
        else {
            std::deque<Node*> mydeck;
            Node* temp = rootNode->child;
            Node* origRoot = rootNode;
            while (temp != nullptr) {
                mydeck.push_back(temp);
                temp = temp->sibling;
            }
            Node *meldOne;
            Node *meldTwo;
            while (mydeck.size() != 1)  {
                meldOne = mydeck.front();
                meldOne->parent = nullptr;
                meldOne->sibling = nullptr;
                mydeck.pop_front();
                
                meldTwo = mydeck.front();
                meldTwo->parent = nullptr;
                meldTwo->sibling = nullptr;
                mydeck.pop_front();
                
                mydeck.push_back(meld(meldOne, meldTwo));
            }
            rootNode = mydeck.front();
            delete origRoot;
            --numNodes;
        }
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the heap.  This should be a reference for speed.  It MUST be
    //              const because we cannot allow it to be modified, as that
    //              might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        return rootNode->elt;
    } // top()


    // Description: Get the number of elements in the priority_queue.
    // Runtime: O(1)
    virtual std::size_t size() const {
        return numNodes;
    } // size()

    // Description: Return true if the priority_queue is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        return rootNode == nullptr;
    } // empty()


    // Description: Updates the priority of an element already in the priority_queue by
    //              replacing the element refered to by the Node with new_value.
    //              Must maintain priority_queue invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more extreme
    //               (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    void updateElt(Node* node, const TYPE &new_value) {
        node->elt = new_value;
        if (this->compare(node->parent->elt, node->elt)) {
            if (node->parent->child == node) {
                node->parent->child = node->sibling;
                node->parent = nullptr;
                node->sibling = nullptr;
                rootNode = meld(rootNode, node);
            }
            else {
                Node* temp = node->parent->child;
                while (temp != node) {
                    if (temp->sibling == node) {
                        temp->sibling = node->sibling;
                        node->sibling = nullptr;
                        node->parent = nullptr;
                        rootNode = meld(rootNode, node);
                        break;
                    }
                    temp = temp->sibling;
                }
            }
        }
    } // updateElt()


    // Description: Add a new element to the priority_queue. Returns a Node* corresponding
    //              to the newly added element.
    // Runtime: O(1)
    // NOTE: Whenever you create a node, and thus return a Node *, you must be sure to
    //       never move or copy/delete that node in the future, until it is eliminated
    //       by the user calling pop().  Remember this when you implement updateElt() and
    //       updatePriorities().
    Node* addNode(const TYPE &val) {
        if (rootNode == nullptr) {
            Node* myNode = new Node(val);
            rootNode = myNode;
            numNodes++;
            return myNode;
        }
        else { // CAll meld
            Node* myNode = new Node(val);
            rootNode = meld(rootNode, myNode);
            numNodes++;
            return myNode;
        }
    } // addNode()


private:
    
    Node* rootNode;
    size_t numNodes;
    //ADD THESE INTO ALL THE CONSTRUCTORs

    
    Node* meld(Node *A, Node* B) {
        if (A == nullptr) {
            rootNode = B;
            return B;
        }
        else if (B == nullptr) {
            rootNode = A;
            return A;
        }
        else if (A->child == nullptr && B->child == nullptr) {
            if (this->compare(A->elt, B->elt)) {
                B->child = A;
                A->parent = B;
                return B;
            }
            else {
                A->child = B;
                B->parent = A;
                return A;
            }
        }
        else { // More than one node, sibling
            if (this->compare(A->elt, B->elt)) {
                A->sibling = B->child;
                A->parent = B;
                B->child = A;
                return B;
            }
            else {
               B->sibling = A->child;
               B->parent = A;
               A->child = B;
               return A;
            }
        }
    }
};


#endif // PAIRINGPQ_H
