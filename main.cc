#include "veb.h"


int main(int argc, char* argv[])
{
    std::cout << "Start" << std::endl;
    veb<int> test;

    for(int i = 0; i < 50; ++i)
    {
        test.insert(i);
    }

    std::cout << "suc 5:"<< test.successor(5) << std::endl;
    std::cout << "THE VERY END" << std::endl;
}
