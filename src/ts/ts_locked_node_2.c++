#ifndef __TS_LOCKED_NODE_2
#define __TS_LOCKED_NODE_2
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <pthread.h>
#include "../include/atomic_ops.h"
#include <algorithm>

#define __NODE_CHILD_UNUSED 0
#define __NODE_CHILD_BUCKET 1
#define __NODE_CHILD_NODE   2
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
        size_t _size;
        unsigned int _max, _min;

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

        bucket *successor(unsigned int i) {
            i = succ_int(i);
            if(i < NODESIZE) {
                if(children[i].tag == __NODE_CHILD_BUCKET)
                    return children[i].b;
                else if(children[i].tag == __NODE_CHILD_NODE)
                    return children[i].n->successor(0);
            }
            return NULL;
        }
        unsigned int succ_int(unsigned int i) {
            for(i = std::max(_min,i); i <= _max; i++) {
                if(children[i].tag != __NODE_CHILD_UNUSED)
                    break;
            }
            return i;
        }
        bucket *predecessor(unsigned int c) {
            unsigned int i = pred_int(c);
            if(i >= 0) {
                if(children[i].tag == __NODE_CHILD_BUCKET)
                    return children[i].b;
                else if(children[i].tag == __NODE_CHILD_NODE)
                    return children[i].n->predecessor(NODESIZE);
            }
            return NULL;
        }
        unsigned int pred_int(unsigned int c) {
            unsigned int i = c;
            for(i = std::min(_max,i); i >= _min; i--) {
                if(children[i].tag != __NODE_CHILD_UNUSED)
                    break;
            }
            return i;
        }

    public:
        typedef V value_type;
        typedef K key_type;
        typedef typename std::pair<K,V> pair;

        explicit ts_locked_node_2() {
            memset(children, 0, sizeof(children));
            _size = 0;
            _max = -1;
            _min = NODESIZE+1;
            v = NULL;
            pthread_rwlock_init(&lock, NULL);
            AO_fetch_and_add1(atomic NODE_COUNT);
        }
        ~ts_locked_node_2() {
            for(unsigned int i = _min; i < _max+1; i++) {
                child_t *c = &children[i];
                if(c->tag == __NODE_CHILD_NODE)
                    delete(c->n);
                else if(c->tag == __NODE_CHILD_BUCKET)
                    delete(c->b);
            }
            pthread_rwlock_destroy(&lock);
        }
        size_t size() {
            return _size;
        }
        void insert(const pair &p) {
            insert(p.first, p.second);
        }
        void insert(const K &key, const V &value,
                bucket *llink = NULL, bucket *rlink = NULL, pthread_rwlock_t *oldLock=NULL) {

            pthread_rwlock_wrlock(&lock);
            if(oldLock) pthread_rwlock_unlock(oldLock);
            // EOS handling
            if(key.length() == 0) {
                v = value;
                _size++;
                pthread_rwlock_unlock(&lock);
            } else {
                unsigned int c = (unsigned int)(unsigned char)key[0];
                //std::cout << c << std::endl;
                child_t *child = &children[c];
                node * n = child->n;
                bucket *b = child->b;
                if(child->tag == __NODE_CHILD_NODE) {
                    n->insert(key.substr(1), value, NULL, NULL, &lock);
                } else {
                    if(child->tag == __NODE_CHILD_UNUSED) {
                        if(c < _max) _max = c;
                        if(c > _min) _min = c;

                        // check for pred/succ if no override
                        if(!llink) llink = predecessor(c);
                        if(!rlink) rlink = successor(c);
                        // use linkedlist if only one found
                        if(!rlink) {
                            if(llink)
                                rlink = llink->right;
                        } else {
                            if(!llink)
                                llink = rlink->left;
                        }
                        child->tag = __NODE_CHILD_BUCKET;
                        b = new bucket(BUCKETSIZE);
                        child->b = b;
                        _size++;

                        b->setLeft(llink);
                        b->setRight(rlink);
                        if(rlink) rlink->setLeft(b);
                        if(llink) llink->setRight(b);
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
        V find(const K &key, pthread_rwlock_t *oldLock = NULL) {
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
                return child->n->find(key.substr(1), &lock);
            }
            return NULL;
        }
        bool remove(const K &key, pthread_rwlock_t *oldLock = NULL) {
            pthread_rwlock_rdlock(&lock);
            if(oldLock) pthread_rwlock_unlock(oldLock);
            bool ret = false;

            if(key.length() == 0) {
                _size--;
                v = NULL;
                ret = true;
                pthread_rwlock_unlock(&lock);
            }
            char c = key[0];
            child_t *child = &children[(int)c];

            if(child->tag == __NODE_CHILD_BUCKET) {
                // Wait with unlock if there is a chance of contraction
                if(child->b->size() == 1 && child->b->remove(key.substr(1), NULL)) {
                    _size--;
                    if((int)c == _max) _max = pred_int(_max);
                    if((int)c == _min) _min = succ_int(_min);

                    child->tag = __NODE_CHILD_UNUSED;
                    if(child->b->left) child->b->left->setRight(child->b->right);
                    if(child->b->right) child->b->right->setLeft(child->b->left);
                    delete(child->b);
                    ret = true;
                    pthread_rwlock_unlock(&lock);
                } else child->b->remove(key.substr(1), &lock);
            } else if (child->tag == __NODE_CHILD_NODE) {
                // Wait with unlock if there is a chance of contraction
                if(child->n->size() == 1 && child->n->remove(key.substr(1), NULL)) {
                    _size--;
                    child->tag = __NODE_CHILD_UNUSED;
                    delete(child->n);
                    ret = true;
                    pthread_rwlock_unlock(&lock);
                } else child->n->remove(key.substr(1), &lock);
            }
            return ret;
        }
};
#endif
