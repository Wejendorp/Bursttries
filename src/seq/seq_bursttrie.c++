#include "seq_node.c++"
#include "seq_sorted_bucket.c++"

#ifndef __SEQ_BURSTTRIE
#define __SEQ_BURSTTRIE

template<
    typename N // Nodetype
>
class seq_bursttrie {
    private:
        N *root;

    public:
        typedef typename N::key_type    K;
        typedef typename N::value_type  V;
        typedef typename N::bucket_type B;

        explicit seq_bursttrie() {
            root = new N();
        }
        ~seq_bursttrie() {
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
