#include <stdio.h>
#include <string.h>

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
                ts_node *n;
                B       *b;
            };
        };

        child_t children[NODESIZE];
        V* v;

    public:
        explicit ts_node() {
            memset(children, 0, sizeof(children));
            v = NULL;
        }
        ~ts_node() { }

        void insert(K key, V* value) {
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
                    child.b = new B;
                }
                child.b->insert(key.substr(1), value);
            }
        }
        V* find(K key, V* value) {
            // EOS handling
            char c = key[0];
            if(c == '\0')
                return v;

            child_t child = children[c];
            if(child.tag == ts_CHILD_BUCKET)
                return child.b->find(key.substr(1), value);
            else if (child.tag == ts_CHILD_NODE) {
                return child.n->find(key.substr(1), value);
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
