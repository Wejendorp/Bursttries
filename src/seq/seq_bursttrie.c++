#ifndef __SEQ_BURSTTRIE
#define __SEQ_BURSTTRIE
#include "seq_node.c++"

template<
    typename K,
    typename V,
    template<class> class B,
    template<class, class, template<class> class> class N // Nodetype
>
class seq_bursttrie {
    private:
        typedef N<K,V,B> node;
        typedef B<node> bucket;
        typedef typename std::pair<K,V> pair;
        node *root;

    public:
        typedef K  key_type;
        typedef V  value_type;

        explicit seq_bursttrie() {
            root = new node();
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
        void insert(pair p) {
            return root->insert(p);
        }
};

#endif
