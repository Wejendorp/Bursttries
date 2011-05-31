#ifndef __TS_BTREE_BUCKET
#define __TS_BTREE_BUCKET

/*
 * void iterativeInorder(Node* root) {
  stack<Node*> nodeStack;
  Node *curr = root;
  for (;;) {
    if (curr != NULL) {
      nodeStack.push(curr);
      curr = curr->left;
      continue;
    }   
    if (nodeStack.size() == 0) {
      return;
    }   
    curr = nodeStack.top();
    nodeStack.pop();
    cout << "Node data: " << curr->data << endl;
    curr = curr->right;
  }
}
 */

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
        K key;
        V value;
        //typedef 
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
            if(k == key)
                value = v;
            else if(k < key) {
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

template<
    typename N
>
class ts_btree_bucket {
    public:
        typedef typename N::value_type value_type;
        typedef typename N::key_type key_type;

    private:
        typedef btree_node<key_type,value_type> btnode;
        typedef N node;
        volatile int capacity, size;
        btnode* root;
        pthread_rwlock_t rwlock;

    public:
        explicit ts_btree_bucket(int cap) {
            capacity = cap;
            root = NULL;
            size = 0;
            pthread_rwlock_init(&rwlock, NULL);
            AO_fetch_and_add1(atomic BUCKET_COUNT);
        }
        ~ts_btree_bucket() {
            AO_fetch_and_add1(atomic BUCKETS_DESTROYED);
            if(root) delete(root);
            pthread_rwlock_destroy(&rwlock);
        }
        void insert(key_type k, value_type v, pthread_rwlock_t *oldLock = NULL) {
            pthread_rwlock_wrlock(&rwlock);
            if(oldLock) pthread_rwlock_unlock(oldLock);
            size++;
            if(root == NULL) root = new btnode(k, v);
            else root->insert(k,v);
            pthread_rwlock_unlock(&rwlock);
        }
        bool remove(key_type key, pthread_rwlock_t *oldLock = NULL) {
            bool ret = false;
            pthread_rwlock_wrlock(&rwlock);
            if(oldLock) pthread_rwlock_unlock(oldLock);
            if(root != NULL) ret = root->remove(key);
            if(ret) size--;
            pthread_rwlock_unlock(&rwlock);
            return ret;
        }
        value_type find(const key_type &key, pthread_rwlock_t *oldLock = NULL) {
            value_type ret = NULL;
            pthread_rwlock_rdlock(&rwlock);
            if(oldLock) pthread_rwlock_unlock(oldLock);
            if(root != NULL) ret = root->find(key);
            pthread_rwlock_unlock(&rwlock);
            return ret;
        }
        node *burst() {
            node *newnode = NULL;
            pthread_rwlock_wrlock(&rwlock);
            if(size > capacity) {
                newnode = new node();
                inOrderBurst(root, newnode);
                size = 0;
            }
            pthread_rwlock_unlock(&rwlock);
            return newnode;
        }
    private:
        void inOrderBurst(btnode *bn, node *nn) {
            if(bn == NULL) return;
            inOrderBurst(bn->left, nn);
            nn->insert(bn->key, bn->value);
            inOrderBurst(bn->right, nn);
        }
};
#endif
