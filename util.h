#ifndef util_h
#define util_h

#include <algorithm>
#include <unordered_set>

template<typename T>
T high(T x, T w_size)
{
    return x >> w_size/2;
}

template<typename T>
T low(T x, T w_size)
{
    return x&((T(1)<<w_size/2)-1);
}

template<typename T>
std::vector<int> get_impor_bits(std::vector<T> keys)
{
    std::vector<int> ret;
    std::unordered_set<int> check;
    for(int i = 0; i < keys.size(); ++i)
    {
        for(int j = 0; j < keys.size(); ++j)
        {
            T xored = keys[i] ^ keys[j];

            if(xored)
            {
                int msb;
                switch(sizeof(xored))
                {
                    case 4:
                        msb = __builtin_clzl(xored);
                        break;
                    case 8:
                        msb = __builtin_clzll(xored);
                        break;
                }

                
                // minus 1 for 0 indexed
                int val = sizeof(xored)*8-msb-1;
                if(check.count(val) == 0)
                {
                    ret.push_back(val);
                    check.insert(val);
                }
            }
        }
    }

    sort(ret.begin(), ret.end());
    return ret;
}

#endif //util_h
