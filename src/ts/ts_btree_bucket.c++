#ifndef __TS_BTREE_BUCKET
#define __TS_BTREE_BUCKET
#include "../seq/btree.c++"

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
        volatile size_t capacity, _size;
        btnode* root;
        pthread_rwlock_t rwlock;

    public:
        typedef typename btnode::iterator iterator;
        // CONSTRUCTOR/DESTRUCTOR
        explicit ts_btree_bucket(int cap) {
            capacity = cap;
            root = NULL;
            _size = 0;
            pthread_rwlock_init(&rwlock, NULL);
            AO_fetch_and_add1(atomic BUCKET_COUNT);
        }
        ~ts_btree_bucket() {
            AO_fetch_and_add1(atomic BUCKETS_DESTROYED);
            if(root) delete(root);
            pthread_rwlock_destroy(&rwlock);
        }

        // ITERATORS
        // start a forward-iterator of the bucket
        iterator begin() {
            return iterator(root);
        }
        // return an (invalid) f-iterator to the element past the
        // end of the bucket.
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
        void insert(key_type k, value_type v, pthread_rwlock_t *oldLock = NULL) {
            pthread_rwlock_wrlock(&rwlock);
            if(oldLock) pthread_rwlock_unlock(oldLock);
            _size++;
            if(root == NULL) root = new btnode(k, v);
            else root->insert(k,v);
            pthread_rwlock_unlock(&rwlock);
        }
        bool remove(const key_type &key, pthread_rwlock_t *oldLock = NULL) {
            bool ret = false;
            pthread_rwlock_wrlock(&rwlock);
            if(oldLock) pthread_rwlock_unlock(oldLock);
            if(root != NULL) ret = root->remove(key);
            if(ret) _size--;
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
            if(_size > capacity) {
                newnode = new node();
                inOrderBurst(root, newnode);
                _size = 0;
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
