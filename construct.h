#ifndef _TINY_CONSTRUCT_H_
#define _TINY_CONSTRUCT_H_

#include <new>
#include <algorithm>

using std::move;

namespace tinySTL {
    template <class T>
    void construct(T* ptr) {
        ::new ((void*)ptr) T();
    }

    template <class T>
    void construct(T* ptr, const T& value) {
        ::new ((void*)ptr) T(value);
    }

    template <class T>
    void construct(T* ptr, T&& value) {
        ::new ((void*)ptr) T(move(value));
    }

    template <class T>
    void destroy(T* ptr) {
        ptr -> ~T();
    }

    template <class T>
    void destroy(T* first, T* last) {
        while (first != last) {
            first -> ~T();
            ++first;
        }
    }
}

#endif // _TINY_CONSTRUCT_H_