#ifndef __SEQ_MAP_BUCKET
#define __SEQ_MAP_BUCKET
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <map>

template<
    typename N
>
class seq_map_bucket {
    public:
        typedef typename N::value_type value_type;
        typedef typename N::key_type key_type;
        typedef N node;
    private:
        typedef typename std::map<key_type,value_type> theMap;
        unsigned int capacity, size;
        typedef seq_map_bucket<N> bucket;
        theMap * m;
    public:
        typedef typename theMap::iterator iterator;
        bucket *left, *right;

        explicit seq_map_bucket(int cap) {
            capacity = cap;
            size = 0;
            left = NULL;
            right = NULL;
            m = new theMap;
        }
        ~seq_map_bucket() {
            if(m) delete(m);
        }
        void insert(key_type k, value_type v) {
            (*m)[k] = v;
        }
        bool remove(key_type key) {
            return false;
        }
        value_type find(key_type key) {
            return (*m)[key];
        }
        node *burst() {
            node * newnode = NULL;
            if(m->size() > capacity) {
                newnode = new node();
                typename theMap::iterator it;
                for(it = m->begin(); it != m->end(); it++) {
                    newnode->insert((*it).first, (*it).second, left, right);
                    left = NULL;
                    right = NULL;
                }
            }
            return newnode;
        }
        iterator begin() {
            return m->begin();
        }
        iterator end() {
            return m->end();
        }
};
#endif
