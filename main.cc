#include "veb.h"
#include "fusion.h"
#include <set>
#include "util.h"

int main(int argc, char* argv[])
{
    //FusionTree<unsigned long long> test;
    
    // veb<unsigned long long> test;
    // srand(time(NULL));
    // std::cout << "Start Inserts" << std::endl;
    // for(int i = 0; i < 500000; ++i)
    // {
    //     test.insert(rand());
    // }
    
    // unsigned long long next = rand();
    // std::cout << "Start successor" << std::endl;
    // std::cout << "suc " << next << ":" << test.successor(next) << std::endl;

    // std::set<unsigned long long> rbTree;

    // std::cout << "Start RB Tree" << std::endl;
    // for(int i = 0; i < 500000; ++i)
    // {
    //     rbTree.insert(rand());
    // }
    // unsigned  long long n = rand();
    // std::cout << "Start successor" << std::endl;
    // unsigned long long suc = *(rbTree.upper_bound(n));
    // std::cout << "suc " << n << ":" << suc << std::endl;


    std::cout << "Test important bits" << std::endl;
    std::vector<unsigned long long> keys;
    keys.push_back(16);
    keys.push_back(20);
    keys.push_back(21);
    keys.push_back(29);
    std::vector<int> msbs = get_impor_bits(keys);
    for(int i = 0; i < msbs.size(); ++i)
    {
        std::cout << msbs[i] << std::endl;
    }

    unsigned long long mask = get_mask<unsigned long long>(msbs);
    std::cout << "mask:" << mask << std::endl;
}
