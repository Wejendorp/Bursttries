#include <stdio.h>
#include <cstdlib> // malloc
#include <iostream>
#include <string.h>
#include <pthread.h>
#include "../include/atomic_ops.h"

#ifndef __TS_LOCKFREE_NODE
#define __TS_LOCKFREE_NODE


#define __NODE_CHILD_UNUSED 0
#define __NODE_CHILD_BUCKET 1
#define __NODE_CHILD_NODE   2
#define NODESIZE 128
#define atomic (volatile AO_t *)

template<
    typename K,
    typename V,
    template<class> class B
>
class ts_lockfree_node {
    private:
        typedef ts_lockfree_node<K,V,B> node;
        typedef B<node> bucket;

        struct child_t {
            char tag; // 
            union {
                node    *n;
                bucket  *b;
            };
        };
        child_t *children[NODESIZE];
        V v;

    public:
        typedef V value_type;
        typedef K key_type;
        typedef typename std::pair<K,V> pair;

        explicit ts_lockfree_node() {
            memset(children, 0, sizeof(children));
            v = NULL;
            AO_fetch_and_add1(atomic NODE_COUNT);
        }
        ~ts_lockfree_node() {
            for(int i = 0; i < NODESIZE; i++) {
                child_t *c = children[i];
                if(c && c->tag == __NODE_CHILD_NODE)
                    delete(c->n);
                else if(c && c->tag == __NODE_CHILD_BUCKET)
                    delete(c->b);
            }
        }
        void insert(const pair &p) {
            insert(p.first, p.second);
        }
        void insert(const K &key, const V &value, unsigned int index = 0) {
            // EOS handling
            if(key.length() == 0) { //<= index) {
                //AO_store_release(atomic &v, (AO_t) value);
                v = value;
            } else {
                char c = key[0];
                child_t **childaddr = &children[(int)c];
                child_t *child = (child_t *)AO_load(atomic childaddr);

                if(child && child->tag == __NODE_CHILD_NODE) {
                    child->n->insert(key.substr(1), value, index+1);
                } else {
                    if(child == NULL) {
                        child = (child_t*)malloc(sizeof(child_t));
                        child->tag = __NODE_CHILD_BUCKET;
                        child->b = new bucket(BUCKETSIZE);
                        AO_store_release(atomic childaddr, (AO_t)child);
                    }
                    child->b->insert(key.substr(1), value, NULL);

                    node *newnode;
                    if((newnode = child->b->burst()) != NULL) {
                        bucket *b = child->b;
                        child = (child_t*)malloc(sizeof(child_t));
                        child->tag = __NODE_CHILD_NODE;
                        child->n = newnode;
                        AO_store_release(atomic childaddr, (AO_t)child);
                        delete(b);
                    }
                }
            }
        }
        V find(const K &key, unsigned int index = 0) {
            if(key.length() == 0) { //<= index) {
                return v;
            }
            char c = key[0];
            child_t *child = children[(int)c];

            if(child->tag == __NODE_CHILD_BUCKET)
                return child->b->find(key.substr(1));
            else if (child->tag == __NODE_CHILD_NODE) {
                return child->n->find(key.substr(1), index+1);
            }
            return NULL;
        }
        void remove(const K &key,unsigned int index = 0) {

            if(key.length() <= index) {
                v = NULL;
            }
            char c = key[index];
            child_t child = children[c];

            if(child.tag == __NODE_CHILD_BUCKET)
                return child.b->remove(key.substr(index));
            else if (child.tag == __NODE_CHILD_NODE) {
                return child.n->remove(key, index+1);
            }
        }
};
#endif
