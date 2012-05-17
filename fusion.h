#ifndef fusion_h
#define fusion_h

#include <list>
#include <vector>
#include <memory>
#include <cassert>
#include <iostream>
#include <algorithm>

template<typename E>
class FusionTree
{
private:
    class Node;
    typedef std::vector<std::shared_ptr<Node> > NodeVector;
    typedef std::vector<E> KeyVector;
    class Node
    {
    public:
        // These must be sorted
        KeyVector keys;
        NodeVector children;

        bool isLeaf;
        E n;

        std::vector<int> bs;
        std::vector<int> ms;

        E m;
        E b_mask;
        E bm_mask;
        int sketch_gap;
        E sketches;

        E sketch_maskl;
        E sketch_maskh;

    public:
        Node(E max_keys)
            : keys(max_keys), children(max_keys+1), n(0)
        {
            
        }

        Node()
            : n(0)
        {}
        
    };


    void precompute_node(std::shared_ptr<Node> x)
    {
        if(x->n == 0)
            return;
        x->bs = get_impor_bits(x->keys);
        x->ms = get_m(x->bs, x->m);

        x->b_mask = get_mask<E>(x->bs);
        x->bm_mask = get_combo_mask<E>(x->bs, x->ms);

        x->sketch_gap = x->bs.back()+x->ms.back()-x->bs.front()-x->ms.front();
        x->sketch_gap = x->sketch_gap ? x->sketch_gap : 1; // if only 1 item gap is 0 from prev line

        int size = x->keys.size();
        for(int j = 0; j < x->keys.size(); ++j)
        {
            E sketch = approx_sketch(x->m, x->keys[size-j-1], x->b_mask, x->bm_mask, x->bs.front()+x->ms.front());
            x->sketches |= (sketch | (E(1)<<x->sketch_gap)) << j*(x->sketch_gap+1);
            x->sketch_maskl |= (E(1) << j*(x->sketch_gap+1));
            x->sketch_maskh |= (E(1) << j*(x->sketch_gap+1));
        }
    }
    
public:

    FusionTree()
    {
        E w = sizeof(E)*8;
        // find 5th root
        max_keys = pow(w, .2);
        std::cout << max_keys << std::endl;
        if(max_keys < 2)
        {
            std::cerr << "Fusion trees need word size at least 32. was:"<< w << std::endl;
            abort();
        }
        min_keys = max_keys/2;
        
        root = std::make_shared<Node>(max_keys);
        root->isLeaf = true;
    }

private:
    std::shared_ptr<Node> root;

    E min_keys;
    E max_keys;

    // x is a non full parent, i is the point to split on
    void split_child(std::shared_ptr<Node> x, E i)
    {
        std::shared_ptr<Node> z = std::make_shared<Node>(max_keys);
        std::shared_ptr<Node> y = x->children[i];
        z->isLeaf = y->isLeaf;
        

        // pivot
        // floor(max_keys/2)
        E pivot = (max_keys)/2;

        z->n = max_keys - pivot - 1;
        
        for(E j = 0; j < z->n; ++j)
        {
            z->keys[j] = y->keys[pivot + j];
        }

        if(!y->isLeaf)
        {
            for(E j = 0; j < z->n+1; ++j)
            {
                z->children[j] = y->children[pivot + j+1];
            }
        }

        // This is probably min keys as soon as it's greater than 1
        y->n = max_keys - z->n - 1;

        // Insert z at c[i+1]
        typename NodeVector::iterator nit = x->children.begin();
        nit += i+1;
        x->children.insert(nit, z);
        x->children.erase(--(x->children.end()));

        // Insert pivot at k[i]
        typename KeyVector::iterator kit = x->keys.begin();
        kit += i;
        x->keys.insert(kit, y->keys[pivot]);
        x->keys.erase(--(x->keys.end()));

        x->n++;
    }

    void insert_nonfull(std::shared_ptr<Node> x, E k)
    {
        E i = x->n;

        if(x->isLeaf)
        {
            while(i >= 1 && k < x->keys[i-1])
            {
                x->keys[i] = x->keys[i-1];
                i--;
            }
            x->keys[i] = k;
            x->n++;
        }
        else
        {
            while(i >= 1 && k < x->keys[i-1])
            {
                i--;
            }
            i++;

            if(x->children[i-1]->n == max_keys)
            {
                split_child(x, i-1);
                if( k > x->keys[i-1])
                {
                    i++;
                }
            }
            insert_nonfull(x->children[i-1], k);
        }
    }

    E successor(std::shared_ptr<Node> x, E k)
    {
        E i = 0;
        while(i < x->n && k > x->keys[i])
        {
            i++;
        }
        if( i < x->n && k == x->keys[i])
        {
            return x->keys[i];
        }
        else if(x->isLeaf)
        {
            return x->keys[i];
        }
        else
        {
            return successor(x->children[i], k);
        }
    }

    E fusion_successor(std::shared_ptr<Node> x, E k)
    {
        int i1 = par_comp(x->sketches,
                          approx_sketch(x->m,
                                        k,
                                        x->b_mask,
                                        x->bm_mask,
                                        x->bs.front()+x->ms.front()) * x->sketch_maskl,
                          x->sketch_maskh,
                          x->sketch_maskl,
                          x->n,
                          x->sketch_gap);

        int lcp1 = k ^ x->keys[i1];
        int lcp2 = k ^ x->keys[i1-1];
        int msb1;
        int msb2;
        switch(sizeof(k))
        {

            case 4:
                msb1 = __builtin_clzl(lcp1);
                msb2 = __builtin_clzl(lcp2);
                break;
            case 8:
                msb1 = __builtin_clzll(lcp1);
                msb2 = __builtin_clzll(lcp2);
                break;
        }

        int y = std::max(msb1, msb2);
        y = sizeof(k)*8 - y-1;
        
        E e_mask = E(1) << y;
        e_mask -= 1;
        
        E new_mask = (E(1) << (y-1));
        new_mask ^= E(-1); // hopefully this has one 0
        
        //E e_mask2 = e_mask << sizeof(k)*8 -y-1;
        E e = k | e_mask; 
        if(((E(1) << y-1))&k)
        {
            e &= new_mask;
        }
        else
        {
            e &= E(-1) ^ ((E(1) << (y-1))-1)
        }

        int i = par_comp(x->sketches,
                         approx_sketch(x->m,
                                       k,
                                       x->b_mask,
                                       x->bm_mask,
                                       x->bs.front()+x->ms.front())*x->sketch_maskl,
                         x->sketch_maskh,
                         x->sketch_maskl,
                         k,
                         x->shift_gap);
    }

        
    

    void initialize(std::shared_ptr<Node> x)
    {
        precompute_node(x);
        if(!x->isLeaf)
        {
            for(int i = 0; i < x->n+1; ++i)
            {
                initialize(x->children[i]);
            }
        }
    }


public:
    void insert(E k)
    {
        std::shared_ptr<Node> r = root;
        if(r->isLeaf)
            std::cout << "root is leaf" << std::endl;
        if(root->n == max_keys)
        {
            std::shared_ptr<Node> s = std::make_shared<Node>(max_keys);
            root = s;
            s->isLeaf = false;
            s->n = 0;
            s->children[0] = r;
            split_child(s, 0);
            insert_nonfull(s, k);
        }
        else
        {
            insert_nonfull(r, k);
        }
    }

    E successor(E k)
    {
        return fusion_successor(root, k);//successor(root, k);
    }

    void initialize()
    {
        initialize(root);
    }
};


#endif //fusion_h
