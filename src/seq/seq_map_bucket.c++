#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <map>
#include "seq_node.c++"

#ifndef __SEQ_MAP_BUCKET
#define __SEQ_MAP_BUCKET

template<
    typename K,
    typename V
>
class seq_map_bucket {
    private:
        typedef seq_node<K,V,seq_map_bucket<K,V> > node;
        typedef typename std::map<K,V> theMap;
        int capacity, size;
        theMap * m;

    public:
        typedef V value_type;
        typedef K key_type;

        explicit seq_map_bucket(int cap) {
            capacity = cap;
            size = 0;
            m = new theMap;
        }
        ~seq_map_bucket() {
            if(m) delete(m);
        }
        void insert(K k, V v) {
            (*m)[k] = v;
        }
        bool remove(K key) {
            return false;
        }
        V find(K key) {
            return (*m)[key];
        }
        bool shouldBurst() {
            return size > capacity;
        }
        node *burst() {
            node *newnode = new node();
            typename theMap::iterator it;
            for(it = m->begin(); it != m->end(); it++) {
                newnode->insert((*it).first, (*it).second);
            }
            return newnode;
        }
};
#endif
