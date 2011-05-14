#include "ts_node.c++"
#include "ts_bucket.c++"

#ifndef __TS_BURSTTRIE
#define __TS_BURSTTRIE

template<
    typename N // Nodetype
>
class ts_bursttrie {
    private:
        N *root;

    public:
        typedef typename N::key_type    K;
        typedef typename N::value_type  V;
        typedef typename N::bucket_type B;

        explicit ts_bursttrie() {
            root = new N();
        }
        ~ts_bursttrie() {
            delete(root);
        }
        void remove(K key) {
            return root->remove(key);
        }
        V find(K key) {
            return root->find(key);
        }
        void insert(K key, V value) {
            return root->insert(key, value);
        }
};

#endif
