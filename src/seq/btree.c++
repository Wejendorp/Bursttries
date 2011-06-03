#ifndef __BTREE_
#define __BTREE_

#include <stack>

template<
    typename K,
    typename V
>
class btree_node {
    private:
        typedef  btree_node<K,V> bnode;
        void inNodeDelete(bnode **bn) {
            bnode *n = *bn;
            if(n->left == NULL && n->right == NULL) {
                // removed node has no children
                delete(n);
                *bn = NULL;
            } else if(n->left == NULL) {
                // removed node has one child
                bnode * temp = n;
                *bn = n->right;
                // Dont delete children, change ptr:
                temp->right = NULL;
                delete(temp);
            } else if(n->right == NULL) {
                // removed node has one child
                *bn = n->left;
                // Dont delete children, change ptr:
                n->left = NULL;
                delete(n);
            } else {
                // removed node has two children:
                // subst with predecessor
                bnode *pred = n->left;
                bnode *parent = n;
                while(pred->right != NULL) {
                    parent = pred;
                    pred = pred->right;
                }
                // Swap values and remove pred:
                n->value = pred->value;
                n->key = pred->key;
                inNodeDelete(&(parent->left));
            }
        }
    public:
        // iterator
        template<typename T>
        class btree_iterator {
        private:
            std::stack<bnode*> nodeStack;
            bnode* curr, current;
        public:
            unsigned int nodeindex;
            typedef btree_iterator<T> self_type;
            btree_iterator(bnode *iterroot, unsigned int index = 0) {
                nodeindex = index;
                curr = iterroot;
            }
            bool operator<(self_type other) {
                return this->nodeindex < other.nodeindex;
            }
            bool operator==(self_type other) {
                return this->nodeindex == other.nodeindex;
            }
            bool operator!=(self_type other) {
                return this->nodeindex != other.nodeindex;
            }
            bnode *operator*() {
                return current;
            }
            self_type &operator++() {
                while(curr != NULL) {
                    nodeStack.push(curr);
                    curr = curr->left;
                }
                curr = nodeStack.pop();
                current = curr;
                curr = curr->right;
                nodeindex++;
                return *this;
            }
        };

        K key;
        V value;
        typedef btree_iterator<bnode> iterator;
        bnode *left, *right;
        explicit btree_node(const K &k, const V &val) {
            left = NULL;
            right = NULL;
            value = val;
            key = k;
        }
        ~btree_node() {
            delete(left);
            delete(right);
        }
        void insert(const K &k, const V &v) {
            if(k < key) {
                if(left == NULL)
                    left = new btree_node(k, v);
                else
                    left->insert(k, v);
            }
            else if(right == NULL)
                right = new btree_node(k, v);
            else
                right->insert(k, v);
        }
        V find(const K &k) {
            if(k == key) return value;
            if(k < key)
                if(left != NULL)
                    return left->find(k);
            if(right != NULL)
                return right->find(k);
            return NULL;
        }
        bool remove(const K &k) {
            if(k < key && left != NULL) {
                if(left->k == key) {
                    inNodeRemove(&left);
                    return true;
                } else {
                    return left->remove(k);
                }
            } else if(right != NULL) {
                if(right->k == key) {
                    inNodeRemove(&right);
                    return true;
                } else {
                    return right->remove(k);
                }
            }
            return false;
        }
};
#endif
