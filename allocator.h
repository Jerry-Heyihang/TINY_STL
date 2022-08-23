#ifndef _TINY_ALLOCATOR_H_
#define _TINY_ALLOCATOR_H_

#include <cstddef>
#include <new>
#include <algorithm>

using std::move;

namespace tinySTL {
    template <class T>
    class allocator {
    public:
        typedef T            value_type;
        typedef T*           pointer;
        typedef const T*     const_pointer;
        typedef T&           reference;
        typedef const T&     const_reference;
        typedef size_t       size_type;
        typedef ptrdiff_t    difference_type;

        pointer allocate();
        pointer allocate(size_type n);

        void deallocate(pointer ptr);
        void deallocate(pointer ptr, size_type n);

        void construct(pointer ptr);
        void construct(pointer ptr, const_reference value);
        void construct(pointer ptr, value_type&& value);

        void destroy(pointer ptr);
        void destroy(pointer first, pointer last);
    };

    template <class T>
    T* allocator<T>::allocate()
    {
        return static_cast<pointer>(::operator new(sizeof(T)));
    }

    template <class T>
    T* allocator<T>::allocate(size_type n)
    {
        if (n == 0) {
            return nullptr;
        }
        return static_cast<pointer>(::operator new(n * sizeof(T)));
    }

    template <class T>
    void allocator<T>::deallocate(pointer ptr)
    {
        if (ptr) {
            ::operator delete(ptr);
        }
    }

    template <class T>
    void allocator<T>::deallocate(pointer ptr, size_type n)
    {
        if (ptr) {
            ::operator delete(ptr, n * sizeof(T));
        }
    }

    template <class T>
    void allocator<T>::construct(pointer ptr)
    {
        ::new ((void*)ptr) T();
    }

    template <class T>
    void allocator<T>::construct(pointer ptr, const_reference value)
    {
        ::new ((void*)ptr) T(value);
    }

    template <class T>
    void allocator<T>::construct(pointer ptr, value_type&& value)
    {
        ::new ((void*)ptr) T(move(value));
    }

    template <class T>
    void allocator<T>::destroy(pointer ptr)
    {
        ptr -> ~T();
    }

    template <class T>
    void allocator<T>::destroy(pointer first, pointer last)
    {
        while (first != last) {
            first -> ~T();
            ++first;
        }
    }
}

#endif // _TINY_ALLOCATOR_H_