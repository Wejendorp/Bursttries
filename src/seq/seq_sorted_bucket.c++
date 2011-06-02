#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include "seq_node.c++"

#ifndef __SEQ_SORTED_BUCkey_typeET
#define __SEQ_SORTED_BUCkey_typeET

#define m_pair std::make_pair<key_type, value_type>

#define STABLE 0
#define BURSTING 1


template<
    typename N
>
class seq_sorted_bucket {
    public:
        typedef typename N::value_type value_type;
        typedef typename N::key_type key_type;
        typedef N node;
    private:

        typedef std::vector<std::pair<key_type, value_type> > pairvector;
        typedef std::allocator<pairvector> vector_alloc_t;
        vector_alloc_t vector_allocator;

        typedef typename pairvector::iterator iter;


        unsigned int capacity;

        pairvector *contents;

    public:

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
        void insert(key_type key, value_type value) {
            iter it = iter_at(key, value);
            contents->insert(it, m_pair(key, value));
        }

        // Remove requires locking, because removing an element inside the
        // vector requires moving all subsequent elements back, leaving the
        // vector in an unstable state.
        bool remove(key_type key) {
            iter it = iter_at(key, NULL);
            if((*it).first != key)
                return false;
            contents->erase(it);
            return true;
        }
        value_type find(key_type key) {
            value_type val = NULL;
            iter it = iter_at(key, NULL);
            if((*it).first == key)
                val = (*it).second;
            return val;
        }
        node *burst() {
            node *newnode = NULL;
            if(contents->size() > capacity) {
                newnode = new node();
                iter it;
                for(it = contents->begin(); it != contents->end(); it++) {
                    std::pair<key_type, value_type> p = (*it);
                    newnode->insert(p.first, p.second);
                }
            }
            return newnode;
        }

    private:
        iter iter_at(key_type key, value_type value) {
            return std::lower_bound(contents->begin(), contents->end(),
                        m_pair(key, value));
        }
};
#endif
