#ifndef util_h
#define util_h

template<typename T>
T high(T x, T w_size)
{
    return x >> w_size/2;
}

template<typename T>
T low(T x, T w_size)
{
    return x&((1<<w_size/2)-1);
}

#endif //util_h
