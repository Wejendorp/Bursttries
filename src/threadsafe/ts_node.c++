#include <stdio.h>
#include <string.h>
#ifndef __TS_NODE
#define __TS_NODE


#define ts_CHILD_UNUSED 0
#define ts_CHILD_BUCKET 1
#define ts_CHILD_NODE   2
#define NODESIZE 128

template<
    typename K,
    typename V,
    typename B
>
class ts_node {
    private:
        struct child_t {
            char tag; // 
            union {
                ts_node<K,V,B> *n;
                B       *b;
            };
        };

        child_t children[NODESIZE];
        V v;

    public:
        typedef K key_type;
        typedef V value_type;
        typedef B bucket_type;

        explicit ts_node() {
            memset(children, 0, sizeof(children));
            v = NULL;
        }
        ~ts_node() {
            for(int i = 0; i < NODESIZE; i++) {
                child_t *c = &children[i];
                if(c->tag == ts_CHILD_NODE)
                    delete(c->n);
                else if(c->tag == ts_CHILD_BUCKET)
                    delete(c->b);
            }
        }

        void insert(K key, V value) {
            // EOS handling
            char c = key[0];
            if(c == '\0') {
                v = value;
                return;
            }
            child_t child = children[c];
            if(child.tag == ts_CHILD_NODE) {
                child.n->insert(key.substr(1), value);
            } else {
                if(child.tag == ts_CHILD_UNUSED) {
                    child.tag = ts_CHILD_BUCKET;
                    child.b = new B(1024);
                }
                child.b->insert(key.substr(1), value);
            }
        }
        V find(K key) {
            // EOS handling
            char c = key[0];
            if(c == '\0')
                return v;

            child_t child = children[c];
            if(child.tag == ts_CHILD_BUCKET)
                return child.b->find(key.substr(1));
            else if (child.tag == ts_CHILD_NODE) {
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
            child_t child = children[c];
            if(child.tag == ts_CHILD_BUCKET)
                return child.b->remove(key.substr(1));
            else if (child.tag == ts_CHILD_NODE) {
                return child.n->remove(key.substr(1));
            }
        }
};
#endif
