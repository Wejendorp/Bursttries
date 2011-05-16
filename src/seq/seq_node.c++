#include <stdio.h>
#include <iostream>
#include <string.h>
#ifndef __SEQ_NODE
#define __SEQ_NODE


#define __NODE_CHILD_UNUSED 0
#define __NODE_CHILD_BUCKET 1
#define __NODE_CHILD_NODE   2
#define NODESIZE 128
#define BUCKETSIZE 1024

template<
    typename K,
    typename V,
    typename B
>
class seq_node {
    private:
        struct child_t {
            char tag; // 
            union {
                seq_node<K,V,B> *n;
                B       *b;
            };
        };

        child_t children[NODESIZE];
        V v;

    public:
        typedef K key_type;
        typedef V value_type;
        typedef B bucket_type;

        explicit seq_node() {
            memset(children, 0, sizeof(children));
            v = NULL;
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

        void insert(K key, V value) {
            // EOS handling
            char c = key[0];
            //if(c == '\0') {
            if(key.length() == 0) {
                v = value;
                return;
            }
            child_t *child = &children[c];
            if(child->tag == __NODE_CHILD_NODE) {
                child->n->insert(key.substr(1), value);
            } else {
                if(child->tag == __NODE_CHILD_UNUSED) {
                    child->tag = __NODE_CHILD_BUCKET;
                    child->b = new B(BUCKETSIZE);
                }
                child->b->insert(key.substr(1), value);

                if(child->b->shouldBurst()) {
                    B *temp = child->b;
                    child->n = temp->burst();
                    child->tag = __NODE_CHILD_NODE;
                    delete(temp);
                }
            }
        }
        V find(K key) {
            // EOS handling
            char c = key[0];
            //if(c == '\0') {
            if(key.length() == 0) {
                std::cout << "in-node return at " << key << std::endl;
                return v;
            }

            child_t *child = &children[c];
            if(child->tag == __NODE_CHILD_BUCKET)
                return child->b->find(key.substr(1));
            else if (child->tag == __NODE_CHILD_NODE) {
                return child->n->find(key.substr(1));
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
