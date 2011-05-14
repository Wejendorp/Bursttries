#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include "ts_node.c++"

#ifndef __SEQ_SORTED_BUCKET
#define __SEQ_SORTED_BUCKET

#define m_pair std::make_pair<K, V>

#define STABLE 0
#define BURSTING 1


template<
    typename K,
    typename V
>
class seq_sorted_bucket {
    private:
        typedef std::vector<std::pair<K, V> > pairvector;
        typedef std::allocator<pairvector> vector_alloc_t;
        vector_alloc_t vector_allocator;

        typedef typename pairvector::iterator iter;
        //typedef seq_sorted_bucket_iterator iterator;
        typedef ts_node<K,V,seq_sorted_bucket<K,V> > node;
        int capacity;

        pairvector *contents;
        //iterator seq_sorted_bucket_iterator<seq_sorted_bucket<V,K> >;

        //friend class seq_sorted_bucket_iterator<seq_sorted_bucket<V,K> >;

    public:
        typedef V value_type;
        typedef K key_type;

        explicit seq_sorted_bucket(int cap) {
            contents = vector_allocator.allocate(1);
            vector_allocator.construct(contents, pairvector(0));

            capacity = cap;
        }
        ~seq_sorted_bucket() {
            vector_allocator.destroy(contents);
            vector_allocator.deallocate(contents, 1);
        }

        // Insert requires locking, for the same reasons as remove
        void insert(K key, V value) {
            iter it = iter_at(key, value);
            contents->insert(it, m_pair(key, value));
        }

        // Remove requires locking, because removing an element inside the
        // vector requires moving all subsequent elements back, leaving the
        // vector in an unstable state.
        bool remove(K key) {
            iter it = iter_at(key, NULL);
            if((*it).first != key)
                return false;
            contents->erase(it);
            return true;
        }
        V find(K key) {
            V val = NULL;
            iter it = iter_at(key, NULL);
            if((*it).first == key)
                val = (*it).second;
            return val;
        }
        node *burst() {
            node *newnode = new node();
            iter it;
            for(it = contents->begin(); it != contents->end(); it++) {
                std::pair<K, V> p = (*it);
                newnode->insert(p.first.substr(1), p.second);
            }
            return newnode;
        }

    private:
        iter iter_at(K key, V value) {
            return std::lower_bound(contents->begin(), contents->end(),
                        m_pair(key, value));
        }
};
#endif
