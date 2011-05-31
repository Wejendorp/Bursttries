#include <stdio.h>
#include <iostream>
#include <string.h>
#include <pthread.h>
#include "../include/atomic_ops.h"

#ifndef __TS_LOCKED_NODE_2
#define __TS_LOCKED_NODE_2


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
class ts_locked_node_2 {
    private:
        typedef ts_locked_node_2<K,V,B> node;
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
        typedef V value_type;
        typedef K key_type;
        typedef typename std::pair<K,V> pair;

        explicit ts_locked_node_2() {
            memset(children, 0, sizeof(children));
            v = NULL;
            pthread_rwlock_init(&lock, NULL);
            AO_fetch_and_add1(atomic NODE_COUNT);
        }
        ~ts_locked_node_2() {
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
            insert(p.first, p.second);
        }
        void insert(const K &key, const V &value, pthread_rwlock_t *oldLock=NULL, unsigned int index = 0) {
            pthread_rwlock_wrlock(&lock);
            if(oldLock) pthread_rwlock_unlock(oldLock);
            // EOS handling
            if(key.length() == 0) { //<= index) {
                v = value;
                pthread_rwlock_unlock(&lock);
            } else {
                char c = key[0];
                child_t *child = &children[(int)c];
                node * n = child->n;
                bucket *b = child->b;
                if(child->tag == __NODE_CHILD_NODE) {
                    n->insert(key.substr(1), value, &lock, index+1);
                } else {
                    if(child->tag == __NODE_CHILD_UNUSED) {
                        child->tag = __NODE_CHILD_BUCKET;
                        child->b = new bucket(BUCKETSIZE);
                        b = child->b;
                    }
                    b->insert(key.substr(1), value, NULL);

                    node *newnode;
                    if((newnode = b->burst()) != NULL) {
                        child->n = newnode;
                        child->tag = __NODE_CHILD_NODE;
                        delete(b);
                    }
                    pthread_rwlock_unlock(&lock);
                }
            }
        }
        V find(const K &key, pthread_rwlock_t *oldLock = NULL, unsigned int index = 0) {
            pthread_rwlock_rdlock(&lock);
            if(oldLock) pthread_rwlock_unlock(oldLock);

            if(key.length() == 0) { //<= index) {
                pthread_rwlock_unlock(&lock);
                return v;
            }
            char c = key[0];
            child_t *child = &children[(int)c];

            if(child->tag == __NODE_CHILD_BUCKET)
                return child->b->find(key.substr(1), &lock);
            else if (child->tag == __NODE_CHILD_NODE) {
                return child->n->find(key.substr(1), &lock, index+1);
            }
            std::cout << std::endl;
            return NULL;
        }
        void remove(const K &key, pthread_rwlock_t *oldLock = NULL, unsigned int index = 0) {
            pthread_rwlock_rdlock(&lock);
            if(oldLock) pthread_rwlock_unlock(oldLock);

            if(key.length() <= index) {
                v = NULL;
                pthread_rwlock_unlock(&lock);
            }
            char c = key[index];
            child_t child = children[c];

            if(child.tag == __NODE_CHILD_BUCKET)
                return child.b->remove(key.substr(index), &lock);
            else if (child.tag == __NODE_CHILD_NODE) {
                return child.n->remove(key, &lock, index+1);
            }
        }
};
#endif
