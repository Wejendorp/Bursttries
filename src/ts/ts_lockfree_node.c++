#include <stdio.h>
#include <iostream>
#include <string.h>
#include <pthread.h>
#include "../include/atomic_ops.h"

#ifndef __TS_NODE
#define __TS_NODE


#define __NODE_CHILD_UNUSED 0
#define __NODE_CHILD_BUCKET 1
#define __NODE_CHILD_NODE   2
#define NODESIZE 128
#define atomic (volatile AO_t *)

template<
    typename K,
    typename V,
    typename B
>
class ts_locked_node {
    private:
        typedef ts_locked_node<K,V,B> node;

        struct child_t {
            char tag; // 
            union {
                node *n;
                B       *b;
            };
        };
        child_t children[NODESIZE];
        V v;

    public:
        typedef K key_type;
        typedef V value_type;
        typedef B bucket_type;
        typedef typename std::pair<K,V> pair;

        explicit ts_locked_node() {
            memset(children, 0, sizeof(children));
            v = NULL;
        }
        ~ts_locked_node() {
            for(int i = 0; i < NODESIZE; i++) {
                child_t *c = &children[i];
                if(c->tag == __NODE_CHILD_NODE)
                    delete(c->n);
                else if(c->tag == __NODE_CHILD_BUCKET)
                    delete(c->b);
            }
        }
        void insert(pair p) {
            insert(p.first, p.second);
        }
        void insert(K key, V value, unsigned int index = 0) {
            // EOS handling
            if(key.length() == index) {
                AO_store_release(atomic &v, (size_t)value);
            } else {
                char c = key[index];
                child_t *child = &children[(int)c];
                node * n = (node*)AO_load(atomic &(child->n));
                B *b =     (B*)AO_load(atomic &(child->b));
                char tag = AO_load_read(atomic &(child->tag));
                if(tag == __NODE_CHILD_NODE) {
                    n->insert(key, value, index+1);
                } else {
                    if(tag == __NODE_CHILD_UNUSED) {
                        AO_store_write(atomic &(child->tag), (size_t) __NODE_CHILD_BUCKET);
                        AO_store_write(atomic &(child->b), (size_t) new B(BUCKETSIZE));
                        b = child->b;
                    }
                    if(b->shouldBurst()) {
                        n = b->burst();
                        delete(b);
                        AO_store_release(atomic &child->n, (size_t) n);
                        AO_store_release(atomic &child->tag, (size_t)  __NODE_CHILD_NODE);
                        n->insert(key, value, index+1);
                    } else {
                        b->insert(key.substr(index+1), value);
                    }
                }
            }
        }
        V find(K key) {
            return find(key, NULL);
        }
        V find(K key, pthread_rwlock_t *oldLock) {
            // EOS handling
            char c = key[0];
            //if(c == '\0') {
            if(key.length() == 0) {
                //std::cout << "in-node return at " << key << std::endl;
                return v;
            }
            child_t child = children[c];

            if(child.tag == __NODE_CHILD_BUCKET)
                return child.b->find(key.substr(1), NULL);
            else if (child.tag == __NODE_CHILD_NODE) {
                return child.n->find(key.substr(1), NULL);
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
