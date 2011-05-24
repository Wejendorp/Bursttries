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
        pthread_rwlock_t lock;

    public:
        typedef K key_type;
        typedef V value_type;
        typedef B bucket_type;
        typedef typename std::pair<K,V> pair;

        explicit ts_locked_node() {
            memset(children, 0, sizeof(children));
            v = NULL;
            pthread_rwlock_init(&lock, NULL);
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
        void insert(pair p) {
            insert(p.first, p.second, NULL);
        }
        void insert(K key, V value, pthread_rwlock_t *oldLock) {
            pthread_rwlock_wrlock(&lock);
            if(oldLock) pthread_rwlock_unlock(oldLock);
            // EOS handling
            char c = key[0];
            if(key.length() == 0) {
                AO_store_release(atomic &v, (size_t)value);
                pthread_rwlock_unlock(&lock);
            } else {
                child_t *child = &children[c];
                node * n = child->n;
                B *b = child->b;
                if(child->tag == __NODE_CHILD_NODE) {
                    n->insert(key.substr(1), value, &lock);
                } else {
                    if(child->tag == __NODE_CHILD_UNUSED) {
                        child->tag = __NODE_CHILD_BUCKET;
                        child->b = new B(BUCKETSIZE);
                        b = child->b;
                    }
                    if(child->b->shouldBurst()) {
                        B *temp = child->b;
                        child->n = temp->burst();
                        n = child->n;
                        child->tag = __NODE_CHILD_NODE;
                        delete(temp);
                        n->insert(key.substr(1), value, &lock);
                    } else {
                        b->insert(key.substr(1), value, &lock);
                    }
                }
            }
        }
        V find(K key) {
            return find(K key, NULL);
        }
        V find(K key, pthread_rwlock_t *oldLock) {
            pthread_rwlock_rdlock(&lock);
            if(oldLock) pthread_rwlock_unlock(oldLock);
            // EOS handling
            char c = key[0];
            //if(c == '\0') {
            if(key.length() == 0) {
                //std::cout << "in-node return at " << key << std::endl;
                pthread_rwlock_unlock(&lock);
                return v;
            }
            child_t child = children[c];

            if(child.tag == __NODE_CHILD_BUCKET)
                return child.b->find(key.substr(1), &lock);
            else if (child.tag == __NODE_CHILD_NODE) {
                return child.n->find(key.substr(1), &lock);
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
