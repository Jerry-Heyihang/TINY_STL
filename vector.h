#ifndef _TINY_VECTOR_H_
#define _TINY_VECTOR_H_

#include "allocator.h"
#include "iterator.h"

using tinySTL::allocator;

namespace tinySTL {
    template <class T, class Alloc = allocator<T>>
    class vector {
    public:
        typedef T                       value_type;
        typedef value_type*             pointer;
        typedef value_type*             iterator;
        typedef value_type&             reference;
        typedef size_t                  size_type;
        typedef ptrdiff_t               difference_type;
        typedef allocator<value_type>   data_allocator;

    protected:
        iterator start;
        iterator finish;
        iterator end_of_storage;

        void deallocate();

        void fill_initialize(size_type n, const value_type& value);

        iterator allocate_and_fill(size_type n, const value_type& value);

    public:
        iterator begin() { return start; }
        iterator end() { return finish; }
        size_type size() { return size_type(finish - start); }
        size_type capacity() const { return size_type(end_of_storage - start); }
        bool empty() { return start == finish; }
        reference operator[](size_type n) { return *(start + n); }
        reference front() { return *start; }
        reference back() { return *(finish - 1); }

        vector() { start = finish = end_of_storage = nullptr; }
        vector(size_type n, const value_type& value) { fill_initialize(n, value); }
        vector(int n, const value_type& value) { fill_initialize(n, value); }
        vector(long n, const value_type& value) { fill_initialize(n, value); }
        explicit vector(size_type n) { fill_initialize(n, value_type()); }
        ~vector() { data_allocator::destroy(start, finish); deallocate(); }

        void push_back(const value_type& x);
        void pop_back();
        void erase(iterator position);
        void resize(size_type new_size, const value_type& x);
        void clear();
    };

    template <class T, class Alloc>
    void vector<T, Alloc>::deallocate()
    {
        if (start) {
            data_allocator::deallocate(start, capacity());
        }
    }

    template <class T, class Alloc>
    void vector<T, Alloc>::fill_initialize(size_type n, const value_type& value)
    {
        start = allocate_and_fill(n, value);
        finish = start + n;
        end_of_storage = finish;
    }

    template <class T, class Alloc>
    typename vector<T, Alloc>::iterator vector<T, Alloc>::allocate_and_fill(size_type n, const value_type& value)
    {
        iterator result = data_allocator::allocate(n);
        uninitialized_fill_n(result, n, value);
        return result;
    }

    template <class T, class Alloc>
    void vector<T, Alloc>::push_back(const value_type& x)
    {
        if (finish != end_of_storage) {
            data_allocator::construct(finish, x);
            ++finish;
        }
        else {
            const size_type old_size = size();
            const size_type new_size = old_size ? 2 * old_size : 1;
            iterator new_start = data_allocator::allocate(new_size);
            iterator new_finish = new_start;
            try {
                new_finish = uninitialized_copy(start, finish, new_start);
                data_allocator::construct(new_finish, x);
                ++new_finish;
            }
            catch (...) {
                data_allocator::deallocate(new_start, new_size);
                throw;
            }
            data_allocator::destroy(start, finish);
            deallocate();
            start = new_start;
            finish = new_finish;
            end_of_storage = start + new_size;
        }
    }

    template <class T, class Alloc>
    void vector<T, Alloc>::pop_back()
    {
        data_allocator::destroy(finish - 1);
        --finish;
    }

    template <class T, class Alloc>
    void vector<T, Alloc>::erase(iterator position)
    {
        if (position + 1 != finish) {
            copy(position + 1, finish, position);
        }
        data_allocator::destroy(finish - 1);
        --finish;
    }

    template <class T, class Alloc>
    void vector<T, Alloc>::resize(size_type new_size, const value_type& x)
    {
        if (new_size < size()) {
            data_allocator::destroy(start + new_size, finish);
        }
        else if (new_size > size()) {
            if (new_size > capacity()) {
                const size_type old_size = size();
                const size_type new_capacity = old_size ? 2 * old_size : 1;
                iterator new_start = data_allocator::allocate(new_capacity);
                iterator new_finish = new_start;
                try {
                    new_finish = uninitialized_copy(start, start + new_size, new_start);
                }
                catch (...) {
                    data_allocator::deallocate(new_start, new_capacity);
                    throw;
                }
                data_allocator::destroy(start, finish);
                deallocate();
                start = new_start;
                finish = new_finish;
                end_of_storage = start + new_capacity;
            }
            else {
                uninitialized_fill_n(finish, new_size - size(), x);
            }
        }
    }

    template <class T, class Alloc>
    void vector<T, Alloc>::clear()
    {
        data_allocator::destroy(start, finish);
        finish = start;
    }

}


#endif