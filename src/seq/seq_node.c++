#include <stdio.h>
#include <iostream>
#include <string.h>
#ifndef __SEQ_NODE
#define __SEQ_NODE


#define __NODE_CHILD_UNUSED 0
#define __NODE_CHILD_BUCKET 1
#define __NODE_CHILD_NODE   2
#define NODESIZE 128

template<
    typename K,
    typename V,
    template<class> class B
>
class seq_node {
    private:
        typedef seq_node<K,V,B> node;
        typedef B<node> bucket;
        size_t _size;
        int _max, _min;

        struct child_t {
            char tag; // 
            union {
                node   *n;
                bucket *b;
            };
        };

        child_t children[NODESIZE];
        V v;

        bucket *successor(int i) {
            i = succ_int(i);
            if(i < NODESIZE) {
                if(children[i].tag == __NODE_CHILD_BUCKET)
                    return children[i].b;
                else if(children[i].tag == __NODE_CHILD_NODE)
                    return children[i].n->successor(0);
            }
            return NULL;
        }
        int succ_int(int i) {
            for(i = std::max(_min,i); i <= _max; i++) {
                if(children[i].tag != __NODE_CHILD_UNUSED)
                    break;
            }
            return i;
        }
        bucket *predecessor(int c) {
            int i = pred_int(c);
            if(i >= 0) {
                if (children[i].tag == __NODE_CHILD_BUCKET)
                    return children[i].b;
                else if(children[i].tag == __NODE_CHILD_NODE)
                    return children[i].n->predecessor(NODESIZE);
            }
            return NULL;
        }
        int pred_int(int c) {
            int i = c;
            for(i = std::min(_max,i); i >= _min; i--) {
                if(children[i].tag != __NODE_CHILD_UNUSED)
                    break;
            }
            return i;
        }

    public:
        typedef K key_type;
        typedef V value_type;
        typedef typename std::pair<K,V> pair;

        explicit seq_node() {
            memset(children, 0, sizeof(children));
            v = NULL;
            _size = 0;
            _max = -1;
            _min = NODESIZE+1;
        }
        ~seq_node() {
            for(int i = 0; i < NODESIZE; i++) {
                child_t *c = &children[i];
                if(c->tag == __NODE_CHILD_NODE)
                    delete(c->n);
                else if(c->tag == __NODE_CHILD_BUCKET)
                    delete(c->b);
            }
        }
        size_t size() {
            return _size;
        }
        void insert(const pair &p) {
            insert(p.first, p.second, NULL, NULL);
        }
        void insert(const K &key, const V &value, bucket *llink = NULL, bucket *rlink = NULL) {
            // EOS handling
            if(key.length() == 0) {
                v = value;
                _size++;
                return;
            }
            char c = key[0];
            child_t *child = &children[(int) c];
            bucket *b = child->b;
            if(child->tag == __NODE_CHILD_NODE) {
                child->n->insert(key.substr(1), value);
            } else {
                if(child->tag == __NODE_CHILD_UNUSED) {
                    if((int)c < _max) _max = (int)c;
                    if((int)c > _min) _min = (int)c;

                    // check for pred/succ if no override
                    if(!llink) llink = predecessor((int)c);
                    if(!rlink) rlink = successor((int)c);
                    // use linkedlist if only one found
                    if(!rlink) {
                        if(llink)
                            rlink = llink->right;
                    } else {
                        if(!llink)
                            llink = rlink->left;
                    }

                    child->tag = __NODE_CHILD_BUCKET;
                    child->b = new bucket(BUCKETSIZE);
                    _size++;
                    b = child->b;

                    b->left = llink;
                    b->right = rlink;
                    if(rlink) rlink->left = b;
                    if(llink) llink->right = b;
                }
                b->insert(key.substr(1), value);

                node *newnode = NULL;
                if((newnode = b->burst()) != NULL) {
                    child->n = newnode;
                    child->tag = __NODE_CHILD_NODE;
                    delete(b);
                }
            }
        }
        V find(K key) {
            // EOS handling
            char c = key[0];
            if(key.length() == 0) {
                return v;
            }
            child_t *child = &children[(int)c];
            if(child->tag == __NODE_CHILD_BUCKET)
                return child->b->find(key.substr(1));
            else if (child->tag == __NODE_CHILD_NODE) {
                return child->n->find(key.substr(1));
            }
            return NULL;
        }
        bool remove(K key) {
            bool ret = false;

            char c = key[0];
            if(key.length() == 0) {
                _size--;
                v = NULL;
                return true;
            }
            child_t *child = &children[c];
            if(child->tag == __NODE_CHILD_BUCKET) {
                if(child->b->remove(key.substr(1)) && child->b->size() == 0) {
                        _size--;
                        if((int)c == _max) _max = pred_int(_max);
                        if((int)c == _min) _min = succ_int(_min);

                        child->tag = __NODE_CHILD_UNUSED;
                        if(child->b->left) child->b->left->right = child->b->right;
                        if(child->b->right) child->b->right->left = child->b->left;
                        delete(child->b);
                        ret = true;
                }
            } else if (child->tag == __NODE_CHILD_NODE) {
                if(child->n->remove(key.substr(1)) && child->n->size() == 0) {
                    _size--;
                    child->tag = __NODE_CHILD_UNUSED;
                    delete(child->n);
                    ret = true;
                }
            }
            return ret;
        }
};
#endif
