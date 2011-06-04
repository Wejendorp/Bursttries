#ifndef __SEQ_BTREE_BUCKET
#define __SEQ_BTREE_BUCKET
#include "btree.c++"

template<
    typename N
>
class seq_btree_bucket {
    public:
        typedef typename N::value_type value_type;
        typedef typename N::key_type key_type;
        typedef N node;
    private:
        typedef btree_node<key_type, value_type> btnode;
        size_t capacity, _size;
        btnode * root;
        typedef seq_btree_bucket<N> bucket;
    public:
        bucket *left, *right;
        typedef typename btnode::iterator iterator;
        explicit seq_btree_bucket(int cap) {
            capacity = cap;
            root = NULL;
            left = NULL;
            right = NULL;
            _size = 0;
        }
        ~seq_btree_bucket() {
            if(root) delete(root);
        }

        //ITERATORS
        // start forward-iterator of the bucket
        iterator begin() {
            return iterator(root);
        }
        iterator end() {
            return iterator(root, size());
        }

        // CAPACITY
        //
        bool empty() {
            return _size == 0;
        }
        size_t size() {
            return _size;
        }
        size_t max_size() {
            return capacity;
        }

        // MODIFIERS
        void insert(const key_type &k, const value_type &v) {
            _size++;
            if(root == NULL) root = new btnode(k, v);
            else root->insert(k,v);
        }
        bool remove(const key_type &key) {
            bool ret = false;
            if(root != NULL) ret = root->remove(key);
            if(ret) _size--;
            return ret;
        }
        value_type find(const key_type &key) {
            if(root == NULL) return NULL;
            return root->find(key);
        }
        node *burst() {
            node *newnode = NULL;
            if(_size > capacity) {
                newnode = new node();
                inOrderBurst(root, newnode);
                _size = 0;
            }
            return newnode;
        }
    private:
        void inOrderBurst(btree_node<key_type,value_type> *bn, node *nn) {
            if(bn == NULL) return;
            inOrderBurst(bn->left, nn);
            nn->insert(bn->key, bn->value);
            inOrderBurst(bn->right, nn);
        }
};
#endif
