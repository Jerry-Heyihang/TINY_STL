#ifndef _TINY_ALLOCATOR_H_
#define _TINY_ALLOCATOR_H_

#include <cstddef>
#include <new>
#include <algorithm>
#include "construct.h"

using std::move;
using namespace tinySTL;

namespace tinySTL {
    template <class T>
    class allocator {
    public:
        typedef T                       value_type;
        typedef T*                      pointer;
        typedef const T*                const_pointer;
        typedef T&                      reference;
        typedef const T&                const_reference;
        typedef size_t                  size_type;
        typedef ptrdiff_t               difference_type;

        static pointer allocate();
        static pointer allocate(size_type n);

        static void deallocate(pointer ptr);
        static void deallocate(pointer ptr, size_type n);

        static void construct(pointer ptr);
        static void construct(pointer ptr, const_reference value);
        static void construct(pointer ptr, value_type&& value);

        static void destroy(pointer ptr);
        static void destroy(pointer first, pointer last);
    };

    template <class T>
    T* allocator<T>::allocate() {
        return static_cast<pointer>(::operator new(sizeof(T)));
    }

    template <class T>
    T* allocator<T>::allocate(size_type n) {
        if (n == 0) {
            return nullptr;
        }
        return static_cast<pointer>(::operator new(n * sizeof(T)));
    }

    template <class T>
    void allocator<T>::deallocate(pointer ptr) {
        if (ptr) {
            ::operator delete(ptr);
        }
    }

    template <class T>
    void allocator<T>::deallocate(pointer ptr, size_type n) {
        if (ptr) {
            ::operator delete(ptr, n * sizeof(T));
        }
    }

    template <class T>
    void allocator<T>::construct(pointer ptr) {
        tinySTL::construct(ptr);
    }

    template <class T>
    void allocator<T>::construct(pointer ptr, const_reference value) {
        tinySTL::construct(ptr, value);
    }

    template <class T>
    void allocator<T>::construct(pointer ptr, value_type&& value) {
        tinySTL::construct(ptr, move(value));
    }

    template <class T>
    void allocator<T>::destroy(pointer ptr) {
        tinySTL::destroy(ptr);
    }

    template <class T>
    void allocator<T>::destroy(pointer first, pointer last) {
        tinySTL::destroy(first, last);
    }

}

#endif // _TINY_ALLOCATOR_H_