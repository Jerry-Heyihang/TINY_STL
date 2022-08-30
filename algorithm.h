#ifndef _TINY_ALGORITHM_H_
#define _TINY_ALGORITHM_H_

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
}


#endif // _TINY_ALGORITHMS_H_