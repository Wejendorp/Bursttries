#ifndef __TS_BTREE_BUCKET
#define __TS_BTREE_BUCKET

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
            if(k == key)
                value = NULL;
            else {
                if(k < key)
                    if(left != NULL)
                        return left->remove(k);
                if(right != NULL)
                    return right->remove(k);
            }
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
        bool remove(key_type key) {
            return false;
        }
        value_type find(const key_type &key, pthread_rwlock_t *oldLock = NULL) {
            value_type ret = NULL;
            pthread_rwlock_rdlock(&rwlock);
            if(oldLock) pthread_rwlock_unlock(oldLock);
            if(root != NULL) ret = root->find(key);
            pthread_rwlock_unlock(&rwlock);
            return ret;
        }
        bool shouldBurst() {
            return (size > capacity);
        }
        node *burst() {
            pthread_rwlock_wrlock(&rwlock);
            node *newnode = new node();
            inOrderBurst(root, newnode);
            size = 0;
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
