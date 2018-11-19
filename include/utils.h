#ifndef UTILS_H
#define UTILS_H

#include <functional>

constexpr int MAX_NSW = 7;
constexpr int MAX_IP = 1000;

// Modified from: https://gist.github.com/martinmoene/9410391
template<class T, class Compare>
inline bool between(T const& val, T const& lo, T const& hi, Compare comp)
{
    return comp(lo, val) && comp(val, hi);
}

template<class T>
inline bool between(T const& val, T const& lo, T const& hi)
{
    return between( val, lo, hi, std::less_equal<T>() );
}

#endif //UTILS_H
