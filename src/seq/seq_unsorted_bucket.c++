#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include "seq_node.c++"

#ifndef __SEQ_UNSORTED_BUCKET
#define __SEQ_UNSORTED_BUCKET

#define m_pair std::make_pair<key_type, value_type>

template<
    typename N
>
class seq_unsorted_bucket {
    public:
        typedef typename N::value_type value_type;
        typedef typename N::key_type key_type;
        typedef N node;

        typedef std::pair<key_type, value_type> pair;
        typedef std::vector<pair > pairvector;
        typedef std::allocator<pairvector> vector_alloc_t;
        vector_alloc_t vector_allocator;

        typedef typename pairvector::iterator iterator;
        unsigned int capacity;

        pairvector *contents;
        typedef seq_unsorted_bucket<N> bucket;

        bucket * left, *right;


        explicit seq_unsorted_bucket(int cap) {
            contents = vector_allocator.allocate(1);
            vector_allocator.construct(contents, pairvector(0));
            capacity = cap;
            left = NULL;
            right = NULL;
        }
        ~seq_unsorted_bucket() {
            vector_allocator.destroy(contents);
            vector_allocator.deallocate(contents, 1);
        }
        void insert(key_type key, value_type value) {
            contents->push_back(m_pair(key, value));
        }
        bool remove(key_type key) {
            iterator it;
            for(it = contents->begin(); it != contents->end(); it++) {
                if((*it).first == key){
                    contents->erase(it);
                    return true;
                }
            }
            return true;
        }
        value_type find(key_type key) {
            iterator it;
            for(it = contents->begin(); it != contents->end(); it++) {
                if((*it).first == key) {
                    return (*it).second;
                }
            }
            return NULL;
        }
        node *burst() {
            node *newnode = NULL;
            if(contents->size() > capacity) {
                newnode = new node();
                iterator it;
                for(it = contents->begin(); it != contents->end(); it++) {
                    pair p = (*it);
                    newnode->insert(p.first, p.second);
                }
            }
            return newnode;
        }
        iterator begin() {
            return contents->begin();
        }
        iterator end() {
            return contents->end();
        }
};
#endif
