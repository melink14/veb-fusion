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
std::vector<int> get_impor_bits(const std::vector<T>& keys)
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

template<typename T>
T get_mask(const std::vector<int>& mask_bits)
{
    T ret = 0;
    for(int i = 0; i < mask_bits.size(); ++i)
    {
        ret |= T(1)<<mask_bits[i];
    }

    return ret;
}

template<typename T>
std::vector<int> get_m(const std::vector<int>& b_bits,  T& m)
{
    m = 0;
    int r = b_bits.size();
    int r3 = pow(r, 3);
    std::vector<int> m_bits(r);

    int mt = 0;
    bool okay = false;
    for(int i = 0; i < r; ++i)
    {
        
        while(!okay)
        {
            for(int j = 0; j < i; ++j)
            {
                for(int k = 0; k < r; ++k)
                {
                    for(int l = 0; l < r; ++l)
                    {
                        if(mt == m_bits[j]+b_bits[k]-b_bits[l])
                        {
                            goto while_label;
                        }
                    }
                }
            }
          while_label:
            if(!okay)
            {
                mt++;
            }
            else
            {
                break;
            }
        }

        int mt_final = (sizeof(T)*8 - b_bits[i] + i*r3);
        mt_final = mt_final/r3 * r3;

        mt_final += mt;

        m_bits[i] = mt_final;
        m |= T(1) << mt_final;
    }

    // found m_prime;
    

    return m_bits;
}

#endif //util_h
