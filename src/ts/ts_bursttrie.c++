#ifndef __TS_BURSTTRIE
#define __TS_BURSTTRIE

#include <utility> // pair
// Include all accessible containers here
#include "ts_locked_node.c++"
#include "ts_locked_node_2.c++"
#include "ts_lockfree_node.c++"

#include "../seq/bt_iterator.c++"
#include "ts_btree_bucket.c++"
//#include "ts_linkedlist_bucket.c++"
#include "ts_map_bucket.c++"
#include "ts_sorted_bucket.c++"
#include "ts_unsorted_bucket.c++"

template<
    typename K,
    typename V,
    template<class> class B,
    template<class, class, template<class> class> class N
>
class ts_bursttrie {
    private:
        typedef N<K, V, B> node;
        typedef B<node> bucket;
        typedef std::pair<K,V> pair;
        typedef bt_iterator<bucket> iterator;
        node *root;

    public:
        explicit ts_bursttrie() {
            root = new node();
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
        void insert(pair p) {
            return root->insert(p);
        }
        iterator begin() {
            return iterator(root->successor(0));
        }
        iterator end() {
            return iterator(root->predecessor(999999), true);
        }

};

#endif
