#include "veb.h"
#include "fusion.h"

int main(int argc, char* argv[])
{
    FusionTree<unsigned> test;
    
    //veb<unsigned> test;
    srand(time(NULL));
    std::cout << "Start Inserts" << std::endl;
    for(int i = 0; i < 500000; ++i)
    {
        test.insert(rand());
    }
    
    unsigned next = rand();
    std::cout << "Start successor" << std::endl;
    std::cout << "suc " << next << ":" << test.successor(next) << std::endl;
}
