#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include "seq_node.c++"

#ifndef __SEQ_BTREE_BUCKET
#define __SEQ_BTREE_BUCKET

template<
    typename K,
    typename V
>
class btree_node {
    public:
        K key;
        V value;
        btree_node<K,V> *left;
        btree_node<K,V> *right;
        explicit btree_node(K k, V val) {
            left = NULL;
            right = NULL;
            value = val;
            key = k;
        }
        ~btree_node() {
            delete(left);
            delete(right);
        }
        void insert(K k, V value) {
            if(k <= key) {
                if(left == NULL)
                    left = new btree_node(k, value);
                else
                    left->insert(k, value);
            }
            else if(right == NULL)
                right = new btree_node(k, value);
            else
                right->insert(k, value);
        }
        V find(K k) {
            if(k == key) return value;
            if(k < key)
                if(left != NULL)
                    return left->find(k);
            if(right != NULL)
                return right->find(k);
            return NULL;
        }
};

template<
    typename K,
    typename V
>
class seq_btree_bucket {
    private:
        typedef seq_node<K,V,seq_btree_bucket<K,V> > node;
        int capacity, size;
        btree_node<K,V> * root;

    public:
        typedef V value_type;
        typedef K key_type;

        explicit seq_btree_bucket(int cap) {
            capacity = cap;
            root = NULL;
        }
        ~seq_btree_bucket() {
            if(root) delete(root);
        }
        void insert(K k, V v) {
            if(root == NULL) root = new btree_node<K,V>(k, v);
            else root->insert(k,v);
        }
        bool remove(K key) {
            return false;
        }
        V find(K key) {
            if(root == NULL) return NULL;
            return root->find(key);
        }
        bool shouldBurst() {
            return size > capacity;
        }
        node *burst() {
            node *newnode = new node();
            inOrderBurst(root, newnode);
            return newnode;
        }
    private:
        void inOrderBurst(btree_node<K,V> *bn, node *nn) {
            if(bn == NULL) return;
            inOrderBurst(bn->left, nn);
            nn->insert(bn->key, bn->value);
            inOrderBurst(bn->right, nn);
        }
};
#endif
