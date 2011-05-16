#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include "seq_node.c++"

#ifndef __SEQ_UNSORTED_BUCKET
#define __SEQ_UNSORTED_BUCKET

#define m_pair std::make_pair<K, V>

#define STABLE 0
#define BURSTING 1

//
// Needs a bitvector implementation
//
template<
    typename K,
    typename V
>
class seq_unsorted_bucket {
    private:
        typedef std::vector<std::pair<K, V> > pairvector;
        typedef std::allocator<pairvector> vector_alloc_t;
        vector_alloc_t vector_allocator;

        typedef typename pairvector::iterator iter;
        typedef seq_node<K,V,seq_unsorted_bucket<K,V> > node;
        int capacity;

        pairvector *contents;

    public:
        typedef V value_type;
        typedef K key_type;

        explicit seq_unsorted_bucket(int cap) {
            contents = vector_allocator.allocate(1);
            vector_allocator.construct(contents, pairvector(0));
            capacity = cap;
        }
        ~seq_unsorted_bucket() {
            vector_allocator.destroy(contents);
            vector_allocator.deallocate(contents, 1);
        }
        void insert(K key, V value) {
            contents->push_back(m_pair(key, value));
        }
        bool remove(K key) {
            iter it;
            for(it = contents->begin(); it != contents->end(); it++) {
                if((*it).first == key){
                    contents->erase(it);
                    return true;
                }
            }
            return true;
        }
        V find(K key) {
            iter it;
            for(it = contents->begin(); it != contents->end(); it++) {
                if((*it).first == key) {
                 //   std::cout << (*it).first << " == "<< key << std::endl;
                    return (*it).second;
                }
            }
            std::cout << key << " not found "<< std::endl;
            return NULL;
        }
        bool shouldBurst() {
            return contents->size() > capacity;
        }
        node *burst() {
            node *newnode = new node();
            iter it;
            for(it = contents->begin(); it != contents->end(); it++) {
                std::pair<K, V> p = (*it);
                newnode->insert(p.first, p.second);
            }
            return newnode;
        }
};
#endif
