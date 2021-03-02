// Project identifier: AD48FB4835AF347EB0CA8009E24C3B13F8519882

#ifndef BINARYPQ_H
#define BINARYPQ_H


#include <algorithm>
#include <utility>
#include "Eecs281PQ.h"

// A specialized version of the 'heap' ADT implemented as a binary heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class BinaryPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Description: Construct an empty heap with an optional comparison functor.
    // Runtime: O(1)
    explicit BinaryPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
            TYPE junk;
            data.push_back(junk);
        // TODO: Implement this function.
    } // BinaryPQ


    // Description: Construct a heap out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    // TODO: when you implement this function, uncomment the parameter names.
    template<typename InputIterator>
    BinaryPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
    BaseClass{ comp }, data { start, end } {
        TYPE junk;
        data.push_back(junk);
        iter_swap(data.begin(), data.begin() + data.size() - 1);
        updatePriorities();
        // TODO: Implement this function.
    } // BinaryPQ


    // Description: Destructor doesn't need any code, the data vector will
    //              be destroyed automaticslly.
    virtual ~BinaryPQ() {
    } // ~BinaryPQ()


    // Description: Assumes that all elements inside the heap are out of order and
    //              'rebuilds' the heap by fixing the heap invariant.
    // Runtime: O(n)
    virtual void updatePriorities() {
        // TODO: Implement this function.
        for (size_t i = data.size() - 1; i > 0; --i) {
            fix_down(data, i);
        }
    } // updatePriorities()


    // Description: Add a new element to the heap.
    // Runtime: O(log(n))
    // TODO: when you implement this function, uncomment the parameter names.
    virtual void push(const TYPE &val) {
        // TODO: Implement this function.
        data.push_back(val);
        fix_up(data, size());
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element from
    //              the heap.
    // Note: We will not run tests on your code that would require it to pop an
    // element when the heap is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: O(log(n))
    virtual void pop() {
        // TODO: Implement this function.
        data[1] = data.back();
        data.pop_back();
        fix_down(data, 1);
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the heap.  This should be a reference for speed.  It MUST be
    //              const because we cannot allow it to be modified, as that
    //              might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        // TODO: Implement this function.
        return data[1];
    } // top()


    // Description: Get the number of elements in the heap.
    // Runtime: O(1)
    virtual std::size_t size() const {
        return data.size() - 1;
    } // size()


    // Description: Return true if the heap is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        return size() == 0;
    } // empty()


private:
    // Note: This vector *must* be used your heap implementation.
    std::vector<TYPE> data;

    void fix_up(std::vector<TYPE> &data, size_t index) {
        // swap modified element with parent until root is reached or a parent
        // with a greater than or equal priority is found
        while (index > 1 && this->compare(data[index / 2], data[index])) {
            std::swap(data[index], data[index / 2]);
            index /= 2;
        }
    }
    
    void fix_down(std::vector<TYPE> &data, size_t index) {
        //CHECK THE HEAP SIZE VARIABLE IF IT SHOULD BE - 1
        size_t heap_size = size();
        while (2 * index <= heap_size) {
            // initialize highest priority child to left child
            size_t larger_child = 2 * index;
            // if right child has higher priorty, set larger child to right child
            if (larger_child < heap_size && this->compare(data[larger_child], data[larger_child + 1])) {
                ++larger_child;
            }
            // if children all have lower priority, the heap is restored
            // so break out of the loop
            if (this->compare(data[larger_child], data[index])) {
                break;
            }
            // else, swap the modified element with the largest child
            std::swap(data[index], data[larger_child]);
            // set new index value for next iteration of loop
            index = larger_child;
        }
    }
    // For instance, you might add fixUp() and fixDown().

}; // BinaryPQ


#endif // BINARYPQ_H
