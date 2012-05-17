#ifndef fusion_h
#define fusion_h

#include <list>
#include <vector>
#include <memory>
#include <cassert>
#include <iostream>

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

    public:
        Node(E max_keys)
            : keys(max_keys), children(max_keys+1), n(0)
        {
            
        }

        Node()
            : n(0)
        {}
        
    };

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
        E i = x->n-1;

        if(x->isLeaf)
        {
            while(i >= 0 && k < x->keys[i])
            {
                x->keys[i+1] = x->keys[i];
                i--;
            }
            x->keys[i+1] = k;
            x->n++;
        }
        else
        {
            while(i >= 0 && k < x->keys[i])
            {
                i--;
            }
            i++;

            if(x->children[i]->n == max_keys)
            {
                split_child(x, i);
                if( k > x->keys[i])
                {
                    i++;
                }
            }
            insert_nonfull(x->children[i], k);
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
        return successor(root, k);
    }
};


#endif //fusion_h
