#ifndef _TINY_LIST_H_
#define _TINY_LIST_H_

#include "allocator.h"
#include "iterator.h"

using tinySTL::allocator;

namespace tinySTL {
    template <class T>
    struct __list_node {
        __list_node<T>* prev;
        __list_node<T>* next;
        T data;
    };

    template <class T, class Ref, class Ptr>
    struct __list_iterator {
        typedef __list_iterator<T, T&, T*>          iterator;
        typedef __list_iterator<T, Ref, Ptr>        self;
        typedef bidirectional_iterator_tag          iterator_category;
        typedef T                                   value_type;
        typedef Ptr                                 pointer;
        typedef Ref                                 reference;
        typedef __list_node<T>*                     link_type;
        typedef size_t                              size_type;
        typedef ptrdiff_t                           difference_type;

        link_type node;

        __list_iterator(link_type x) : node(x) {}
        __list_iterator() {}
        __list_iterator(const iterator& x) : node(x.node) {}

        bool operator==(const self& x) const { return node == x.node; }
        bool operator!=(const self& x) const { return node != x.node; }

        reference operator*() const { return (*node).data; }

        pointer operator->() const { return &(operator*()); }

        self& operator++() {
            node = (link_type)(node -> next);
            return *this;
        }

        self operator++(int) {
            self tmp = *this;
            ++*this;
            return tmp;
        }

        self& operator--() {
            node = &((*node)->prev);
            return *this;
        }

        self operator--(int) {
            self tmp = *this;
            --*this;
            return tmp;
        }
    };

    template <class T, class Alloc = allocator<T>>
    class list {
    protected:
        typedef T                                       value_type;
        typedef value_type*                             pointer;
        typedef value_type&                             reference;
        typedef __list_node<T>                          list_node;
        typedef allocator<list_node>                    list_node_allocator;
        typedef __list_iterator<T, T&, T*>              iterator;
        typedef size_t                                  size_type;
        typedef ptrdiff_t                               difference_type;
        typedef list_node*                              link_type;

        link_type                                       node;

        link_type get_node() { return list_node_allocator::allocate(); }
        void put_node(link_type p) { list_node_allocator::deallocate(p); }

        link_type create_node(const T& x) {
            link_type p = get_node();
            construct(&(p -> data), x);
            return p;
        }

        void destroy_node(link_type p) {
            destroy(&(p -> data));
            put_node(p);
        }

        void empty_initialize() {
            node = get_node(); 
            node -> next = node;
            node -> prev = node;
        }

        void transfer(iterator position, iterator first, iterator last) {
            if (position == last) return;
            last.node -> prev -> next = position.node;
            first.node -> prev -> next = last.node;
            position.node -> prev -> next = first.node;
            link_type tmp = position.node -> prev;
            position.node -> prev = last.node -> prev;
            last.node -> prev = first.node -> prev;
            first.node -> prev = tmp;
        }

    public:
        iterator begin() { return (link_type)((*node).next); }
        iterator end() { return node; }
        bool empty() const { return node == node -> next; }

        size_type size() const {
            size_type result = 0;
            distance(begin(), end(), result);
            return result;
        }

        reference front() { return *begin(); }
        reference back() { return *(--end()); }

        list() { empty_initialize(); }

        iterator insert(iterator position, const T& x) {
            link_type tmp = create_node(x);
            tmp -> next = position.node;
            tmp -> prev = position.node -> prev;
            (position.node -> prev) -> next = tmp;
            position.node -> prev = tmp;
            return tmp;
        }
        
        void push_front(const T& x) { insert(begin(), x); }
        void push_back(const T& x) { insert(end(), x); }

        iterator erase(iterator position) {
            link_type next_node = position.node -> next;
            link_type prev_node = position.node -> prev;
            prev_node -> next = next_node;
            next_node -> prev = prev_node;
            destroy_node(position.node);
            return (iterator)next_node;
        }

        void pop_front() { erase(begin()); }
        void pop_back() { erase(--end()); }

        void clear() {
            link_type cur = (link_type) node -> next;
            while (cur != node) {
                link_type tmp = cur;
                cur = (link_type) cur -> next;
                destroy_node(tmp);
            }
            node -> next = node;
            node -> prev = node;
        }

        void remove(const T& x) {
            iterator first = begin();
            iterator last = end();
            while (first != last) {
                iterator next = first;
                ++next;
                if (*first == x) erase(first);
                first = next;
            }
        }

        void unique() {
            iterator first = begin();
            iterator last = end();
            if (first == last) return;
            iterator next = first;
            while (++next != last) {
                if (*first == *next) erase(next);
                else first = next;
                next = first;
            }
        }

        void splice(iterator position, list& x) {
            if (!x.empty()) {
                transfer(position, x.begin(), x.end());
            }
        }

        void splice(iterator position, list&, iterator i) {
            iterator j = i;
            ++j;
            if (position == i || position == j) return;
            transfer(position, i, j);
        }

        void splice(iterator position, list&, iterator first, iterator last) {
            if (first != last) {
                transfer(position, first, last);
            }
        }

        void merge(list<T, Alloc>& x) {
            iterator first1 = begin();
            iterator last1 = end();
            iterator first2 = x.begin();
            iterator last2 = x.end();

            while (first1 != last1 && first2 != last2) {
                if (*first2 < *first1) {
                    iterator next = first2;
                    transfer(first1, first2, ++next);
                    first2 = next;
                } else {
                    ++first1;
                }
            }
            if (first2 != last2) transfer(last1, first2, last2);
        }

        void reverse() {
            if (node -> next == node || link_type(node -> next) -> next == node) return;

            iterator first = begin();
            ++first;
            while (first != end()) {
                iterator old = first++;
                transfer(begin(), old, first);
            }
        }

        void swap(list<T, Alloc>& x) {
            link_type tmp = node;
            node = x.node;
            x.node = tmp;
        }

        void sort() {
            if (node -> next == node || link_type(node -> next) -> next == node) return;
            
            list<T, Alloc> carry;
            list<T, Alloc> counter[64];
            int fill = 0;
            while (!empty()) {
                carry.splice(carry.begin(), *this, begin());
                int i = 0;
                while (i < fill && !counter[i].empty()) {
                    counter[i].merge(carry);
                    carry.swap(counter[i++]);
                }
                carry.swap(counter[i]);
                if (i == fill) ++fill;
            }
            for (int i = 1; i < fill; ++i) {
                counter[i].merge(counter[i - 1]);
            }
            swap(counter[fill - 1]);
        }

    };
}

#endif // _TINY_LIST_H_