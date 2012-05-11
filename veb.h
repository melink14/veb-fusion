#ifndef veb_h
#define veb_h

#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <memory>

#include "util.h"

#define WORD_BITS 8

template< typename E>
class veb
{
    typedef typename std::unordered_map<E, std::shared_ptr<veb<E> > > map_type;
    map_type clusters;
    
    std::shared_ptr<veb<E> > summary;
    E min;  // not stored in a cluster
    E max; // copy of element stored in cluster
    bool isEmpty;

    E w_size; // effective size

    // provide working space

    std::shared_ptr<veb<E> > getSummary()
    {
        if(summary.get() == NULL)
        {
            summary = std::make_shared<veb<E> >(w_size/2);
        }
        return summary;
    }

public:
    veb()
        : isEmpty(true), w_size(sizeof(E)*WORD_BITS)
    {
        
    }

    explicit veb(E w_size_)
    : isEmpty(true), w_size(w_size_)
    {
        
    }
    
    
    
    E successor(E x)
    {
        E w2 = w_size/2;
        std::cout << w2 << std::endl;
        E c = high(x, w_size);
        E i = low(x, w_size);
        
        if(x < min)
        {
            return min;
        }
        // If a cluster which would contain x exists and
        // if i would be less than it's max we find successor
        // inside of that cluster
        typename map_type::const_iterator c_it = clusters.find(c);
        if(c_it != clusters.end() && i < (*c_it).second->max)
        {
            return (c<<w2)|(*c_it).second->successor(i);
        }
        else
        {
            E c_prime = getSummary()->successor(c);
            return (c_prime << w2) | clusters[c_prime]->min;
        }
    }

    //   E predecessor(E x);

    void insert(E x)
    {
        if(isEmpty)
        {
            min = x;
            max = x;
            isEmpty = false;
            return;
        }
        if(x < min)
        {
            std::swap(x, min);
        }
        if(x > max)
        {
            max = x;
        }
        E c = high(x, w_size);
        E i = low(x, w_size);
        if(clusters.count(c) == 0)
        {
            getSummary()->insert(c);
            
            clusters[c] = std::make_shared<veb<E> >(w_size/2);
        }
        clusters[c]->insert(i);
    }

    // E erase(E x);

    E get_max() {return max;}

    E get_min() {return min;}

    bool get_isEmpty() {return isEmpty;}
};

#endif // veb_h
