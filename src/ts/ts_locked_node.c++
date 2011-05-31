#include <stdio.h>
#include <iostream>
#include <string.h>
#include <pthread.h>
#include "../include/atomic_ops.h"

#ifndef __TS_LOCKED_NODE
#define __TS_LOCKED_NODE


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
class ts_locked_node {
    private:
        typedef ts_locked_node<K,V,B> node;
        typedef B<node> bucket;

        struct child_t {
            char tag; // 
            union {
                node    *n;
                bucket  *b;
            };
        };
        child_t children[NODESIZE];
        V v;
        pthread_rwlock_t lock;

    public:
        typedef K key_type;
        typedef V value_type;
        typedef typename std::pair<K,V> pair;

        explicit ts_locked_node() {
            memset(children, 0, sizeof(children));
            v = NULL;
            pthread_rwlock_init(&lock, NULL);
            AO_fetch_and_add1(atomic NODE_COUNT);
        }
        ~ts_locked_node() {
            for(int i = 0; i < NODESIZE; i++) {
                child_t *c = &children[i];
                if(c->tag == __NODE_CHILD_NODE)
                    delete(c->n);
                else if(c->tag == __NODE_CHILD_BUCKET)
                    delete(c->b);
            }
            pthread_rwlock_destroy(&lock);
        }
        void insert(const pair &p) {
            insert(p.first, p.second, NULL);
        }
        void insert(const K &key, const V &value) {
            insert(key, value, NULL);
        }
        void insert(K key, V value, void* temp) {
            pthread_rwlock_wrlock(&lock);
            // EOS handling
            char c = key[0];
            bucket * b;
            if(key.length() == 0) {
                v = value;
            } else {
                child_t *child = &children[(int)c];
                if(child->tag == __NODE_CHILD_NODE) {
                    child->n->insert(key.substr(1), value);
                } else {
                    if(child->tag == __NODE_CHILD_UNUSED) {
                        child->tag = __NODE_CHILD_BUCKET;
                        child->b = new bucket(BUCKETSIZE);
                    }
                    child->b->insert(key.substr(1), value, NULL);

                    node *newnode;
                    b = child->b;
                    if((newnode = child->b->burst()) != NULL) {
                        child->n = newnode;
                        child->tag = __NODE_CHILD_NODE;
                        delete(b);
                    }
                }
            }
            pthread_rwlock_unlock(&lock);
        }
        V find(K key, void* t) {
            return find(key);
        }
        V find(K key) {
            pthread_rwlock_rdlock(&lock);
            // EOS handling
            char c = key[0];
            //if(c == '\0') {
            if(key.length() == 0) {
                //std::cout << "in-node return at " << key << std::endl;
                pthread_rwlock_unlock(&lock);
                return v;
            }
            child_t child = children[(int)c];
            pthread_rwlock_unlock(&lock);

            if(child.tag == __NODE_CHILD_BUCKET)
                return child.b->find(key.substr(1));
            else if (child.tag == __NODE_CHILD_NODE) {
                return child.n->find(key.substr(1));
            }
            return NULL;
        }
        void remove(K key) {
            char c = key[0];
            if(c == '\0') {
                v = NULL;
                return;
            }
            child_t *child = &children[c];
            if(child->tag == __NODE_CHILD_BUCKET)
                return child->b->remove(key.substr(1));
            else if (child->tag == __NODE_CHILD_NODE) {
                return child->n->remove(key.substr(1));
            }
        }
};
#endif
