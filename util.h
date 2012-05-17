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
T get_combo_mask(const std::vector<int>& a, const std::vector<int>& b)
{
    std::vector<int> temp;
    for(int i = 0; i < a.size(); ++i)
    {
        temp.push_back(a[i]+b[i]);
    }

    return get_mask<T>(temp);
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
            okay = true;
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

template<typename T>
T mul_high(T a, T b)
{
    int w = sizeof(a) *8;
    switch(w)
    {
        case 32:
        {
            unsigned long long ret = (unsigned long long)a * b;
            return ret >> w;
            break;
        }
        case 64:
        {
            T a1 = a >> w/2;
            T a0 = a&((T(1)<<w/2)-1);
            T b1 = b >> w/2;
            T b0 = b&((T(1)<<w/2)-1);

            T res = a1*b1;
            res += (a1*b0) >> w/2;
            res += (a0*b1) >> w/2;

            return res;
        }
    }
    
}

template<typename T>
T approx_sketch(T m, T x, T b_mask, T bm_mask, int shift_dis)
{
    if(shift_dis != sizeof(m)*8)
    {
        std::cout << "shift isn't a full word! " << shift_dis  << std::endl;
    }

    T x_prime = x & b_mask;
    
    T as = mul_high(x_prime, m);

    return as & bm_mask;
}

// returns i+1
template<typename T>
int par_comp(T sketch_node, T sketch_k, T sketch_maskh, T sketch_maskl, int k, int gap)
{
    T diff = sketch_node - sketch_k;
    diff &= sketch_maskh;

    diff *= sketch_maskl;

    unsigned mask = (1<<(unsigned)log(k))-1;

    diff = (diff >> gap*k)&mask;

    std::cout << "person after me:" << k-diff << std::endl;
    
    return k-diff;
    
}

#endif //util_h
