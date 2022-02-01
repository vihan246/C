#include <vector>
#include <stdexcept>
#include <map>
template <typename T>
class MinHeap {
    public:
        MinHeap (int inD);
        /* Constructor that builds a d-ary Min Heap
        This should work for any d >= 2,
        but doesn't have to do anything for smaller d.*/
        ~MinHeap ();

        void add (T item, int priority);
            /* adds the item to the heap, with the given priority. */

        const T & peek () const;
            /* returns the element with smallest priority.  
                Break ties however you wish.  
                Throws an exception if the heap is empty. */

        void remove ();
            /* removes the element with smallest priority.
                Break ties however you wish.
                Throws an exception if the heap is empty. */

        bool isEmpty ();
            /* returns true iff there are no elements on the heap. */
        
   
    private:
        // whatever you need to naturally store things.
        // You may also add helper functions here.
        std::vector<std::pair<T, int> > heap;
        int d;
        // trickleDown and bubbleUp ensure that a node is placed in the correct position in the vector. 
        void trickleDown(int pos);
        void bubbleUp(int pos);
        int size;
};

//Constructor that takes in the d of d-ary. 
template <typename T>
MinHeap<T>::MinHeap(int inD)
{
    std::vector<std::pair<T, int> > heap;
    d = inD;
    size=0;
}

// Destructor is empty as no memory has been dynamically allocated. 
template <typename T>
MinHeap<T>::~MinHeap()
{
    
}

// add pushes the pair back into the vector and then calls bubbleUp on that position in the vector. 
template <typename T>
void MinHeap<T>::add(T item, int priority)
{
    heap.push_back(std::pair<T, int>(item, priority));
    size++;
    int position = int(heap.size())-1;
    if (heap.size() > 1)
    {
        bubbleUp(position);
    }
}

// peek just returns the item at the root of the heap. 
template <typename T>
const T & MinHeap<T>::peek() const
{
    if (size == 0)
    {
        throw std::out_of_range("Out of range");
    }
    return heap[0].first; 
}

//remove swaps the first node with the last node and pops back before calling 
// trickle down on the now swapped last node. 
template <typename T>
void MinHeap<T>::remove()
{
    if (size ==0)
    {
        throw std::out_of_range("Out of range");
    }
    std::swap(heap[0],heap[heap.size()-1]);
    heap.pop_back();
    trickleDown(0);
    size--;
}

// is empty returns true if the size of the heap is 0.
template <typename T>
bool MinHeap<T>::isEmpty()
{
    if (size == 0)
    {
        return true;
    }
    return false;
    
}

template <typename T>
void MinHeap<T>::trickleDown(int pos)
{
    //leftMost keeps itself as the position for the first child. C is the one that changes depending
    //on if the next child is smaller. 
    int leftMost = (d*pos) + 1;
    int c = leftMost;
    if (leftMost > int(heap.size())-1)
    {
        return;
    }
    for (int i = 1; i < d; i++)
    {
        if ((leftMost+i) < int(heap.size()))
        {
            int temp = leftMost + i;
            if (heap[c].second > heap[temp].second)
            {
                c = temp;
            }
        }
    }
    if(heap[pos].second > heap[c].second)
    {
        // recursive call ensures that trickle down will continue to be called until the node is in the right 
        // place.
        std::swap(heap[pos], heap[c]);
        trickleDown(c);
    }
    
}

//bubbleUp swaps a child with its parent if the child's priority is lower. 
template <typename T>
void MinHeap<T>::bubbleUp(int pos)
{
    int position = pos;
    while ((position > 0)&&(heap[position].second <= heap[((position-1)/d)].second))
    {
        if (heap[position].second < heap[((position-1)/d)].second)
        {
            std::swap(heap[position], heap[((position-1)/d)]);
        }
        // tie breaker for step 3, checks if the priorities are the same it make the smaller string 
        // have a lower priority. 
        else if (heap[position].first < heap[((position-1)/d)].first)
        {
            std::swap(heap[position], heap[((position-1)/d)]);
        }
        position = ((position-1)/d);
    }
}

