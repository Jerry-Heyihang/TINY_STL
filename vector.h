#ifndef _TINY_VECTOR_H_
#define _TINY_VECTOR_H_

#include "allocator.h"
#include "iterator.h"
#include "algorithm.h"

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
        typedef const value_type*       const_pointer;
        typedef const value_type&       const_reference;
        typedef const value_type*       const_iterator;

    protected:
        iterator start;
        iterator finish;
        iterator end_of_storage;

        void deallocate();

        void fill_initialize(size_type n, const value_type& value);

        iterator allocate_and_fill(size_type n, const value_type& value);

        void swap(vector<T, Alloc>& x);

        iterator allocate_and_copy(size_type n, const_iterator first, const_iterator last);

    public:
        iterator begin() const { return start; }
        iterator end() const { return finish; }
        size_type size() const { return size_type(finish - start); }
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

        vector& operator=(const vector& x);
        vector& operator=(vector&& x) noexcept;
        vector(const vector& x);
        vector(vector&& x) noexcept;
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
    typename vector<T, Alloc>::iterator vector<T, Alloc>::allocate_and_copy(size_type n, const_iterator first, const_iterator last)
    {
        iterator result = data_allocator::allocate(n);
        uninitialized_copy(first, last, result);
        return result;
    }

    template <class T, class Alloc>
    void vector<T, Alloc>::swap(vector<T, Alloc>& x)
    {
        tinySTL::swap(start, x.start);
        tinySTL::swap(finish, x.finish);
        tinySTL::swap(end_of_storage, x.end_of_storage);
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

    template <class T, class Alloc>
    vector<T, Alloc>& vector<T, Alloc>::operator=(const vector& x)
    {
        if (this != &x) {
            const size_type x_size = x.size();
            if (x_size > capacity()) {
                vector<T, Alloc> tmp(x);
                swap(tmp);
            }
            else if (size() >= x_size) {
                iterator i = tinySTL::copy(x.begin(), x.end(), begin());
                data_allocator::destroy(i, finish);
            }
            else {
                copy(x.start, x.start + size(), start);
                uninitialized_copy(x.start + size(), x.finish, finish);
            }
            finish = start + x_size;
        }
        return *this;
    }

    template <class T, class Alloc>
    vector<T, Alloc>& vector<T, Alloc>::operator=(vector&& x) noexcept
    {
        if (this != &x) {
            data_allocator::destroy(start, finish);
            deallocate();
            start = x.start;
            finish = x.finish;
            end_of_storage = x.end_of_storage;
            x.start = x.finish = x.end_of_storage = nullptr;
        }
        return *this;
    }

    template <class T, class Alloc>
    vector<T, Alloc>::vector(const vector& x)
    {
        start = allocate_and_copy(x.end() - x.begin(), x.begin(), x.end());
        finish = start + (x.end() - x.begin());
        end_of_storage = finish;
    }

    template <class T, class Alloc>
    vector<T, Alloc>::vector(vector&& x) noexcept
    {
        start = x.start;
        finish = x.finish;
        end_of_storage = x.end_of_storage;
        x.start = x.finish = x.end_of_storage = nullptr;
    }
}


#endif