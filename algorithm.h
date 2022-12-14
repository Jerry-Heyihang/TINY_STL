#ifndef _TINY_ALGORITHM_H_
#define _TINY_ALGORITHM_H_

#include <utility>

namespace tinySTL {

    template <class RandomAccessIterator>
    void swap(RandomAccessIterator first, RandomAccessIterator second) {
        typename iterator_traits<RandomAccessIterator>::value_type temp = *first;
        *first = *second;
        *second = temp;
    }

    template <class RandomAccessIterator>
    RandomAccessIterator partition(RandomAccessIterator first, RandomAccessIterator last) {
        RandomAccessIterator mid = first + (last - first) / 2;
        swap(first, mid);
        RandomAccessIterator i = first + 1;
        RandomAccessIterator j = last - 1;
        while (i <= j) {
            while (i <= j && *i < *first) {
                ++i;
            }
            while (i <= j && *j > *first) {
                --j;
            }
            if (i < j) {
                swap(i, j);
                ++i;
                --j;
            }
        }
        swap(first, j);
        return j;
    }

    template <class RandomAccessIterator>
    void sort(RandomAccessIterator first, RandomAccessIterator last) {
        if (first != last) {
            RandomAccessIterator mid = partition(first, last);
            sort(first, mid);
            sort(mid + 1, last);
        }
    }

    template <class InputIterator, class OutputIterator>
    OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result) {
        while (first != last) {
            *result = *first;
            ++result;
            ++first;
        }
        return result;
    }

    template <class BidirectionalIterator1, class BidirectionalIterator2>
    BidirectionalIterator2 copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result) {
        while (first != last) {
            --last;
            --result;
            *result = *last;
        }
        return result;
    }

    template <class T>
    typename std::remove_reference<T>::type&& move(T&& t) noexcept {
        return static_cast<typename std::remove_reference<T>::type&&>(t);
    }

    template <class T>
    const T& max(const T& a, const T& b) {
        return a < b ? b : a;
    }
}


#endif // _TINY_ALGORITHMS_H_