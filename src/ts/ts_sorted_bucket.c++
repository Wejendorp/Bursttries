#ifndef __TS_SORTED_BUCKET
#define __TS_SORTED_BUCKET
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <string.h>

#define m_pair std::make_pair<key_type, value_type>

template<
    typename N
>
class ts_sorted_bucket {
    public:
        typedef typename N::value_type value_type;
        typedef typename N::key_type key_type;
        typedef N node;
    private:
        typedef ts_sorted_bucket<N> bucket;
        typedef std::vector<std::pair<key_type, value_type> > pairvector;
        typedef std::allocator<pairvector> vector_alloc_t;
        vector_alloc_t vector_allocator;

        typedef typename pairvector::iterator iter;


        unsigned int capacity;

        pairvector *contents;
        pthread_rwlock_t lock;

    public:
        bucket *left,*right;

        explicit ts_sorted_bucket(int cap) {
            contents = vector_allocator.allocate(1);
            vector_allocator.construct(contents, pairvector(0));
            left = NULL;
            right = NULL;
            capacity = cap;
            pthread_rwlock_init(&lock, NULL);
            AO_fetch_and_add1(atomic BUCKET_COUNT);
        }
        ~ts_sorted_bucket() {
            AO_fetch_and_add1(atomic BUCKETS_DESTROYED);
            vector_allocator.destroy(contents);
            vector_allocator.deallocate(contents, 1);
            pthread_rwlock_destroy(&lock);
        }
        void setLeft(bucket *llink) {
            pthread_rwlock_wrlock(&lock);
            left = llink;
            pthread_rwlock_unlock(&lock);
        }
        void setRight(bucket *rlink) {
            pthread_rwlock_wrlock(&lock);
            right = rlink;
            pthread_rwlock_unlock(&lock);
        }

        // Insert requires locking, for the same reasons as remove
        void insert(const key_type &key, const value_type &value, pthread_rwlock_t *oldLock = NULL) {
            pthread_rwlock_wrlock(&lock);
            if(oldLock) pthread_rwlock_unlock(oldLock);

            iter it = iter_at(key, value);
            contents->insert(it, m_pair(key, value));

            pthread_rwlock_unlock(&lock);
        }

        // Remove requires locking, because removing an element inside the
        // vector requires moving all subsequent elements back, leaving the
        // vector in an unstable state.
        bool remove(const key_type &key, pthread_rwlock_t * oldLock = NULL) {
            pthread_rwlock_wrlock(&lock);
            if(oldLock) pthread_rwlock_unlock(oldLock);
            bool ret = true;

            iter it = iter_at(key, NULL);
            if((*it).first != key)
                ret = false;
            else
                contents->erase(it);

            pthread_rwlock_unlock(&lock);
            return ret;
        }
        value_type find(const key_type &key, pthread_rwlock_t *oldLock = NULL) {
            pthread_rwlock_rdlock(&lock);
            if(oldLock) pthread_rwlock_unlock(oldLock);

            value_type val = NULL;
            iter it = iter_at(key, NULL);
            if((*it).first == key)
                val = (*it).second;

            pthread_rwlock_unlock(&lock);
            return val;
        }
        node *burst() {
            pthread_rwlock_wrlock(&lock);
            node *newnode = NULL;
            if(contents->size() > capacity) {
                newnode = new node();
                iter it;
                for(it = contents->begin(); it != contents->end(); it++) {
                    std::pair<key_type, value_type> p = (*it);
                    newnode->insert(p.first, p.second);
                }
            }
            pthread_rwlock_unlock(&lock);
            return newnode;
        }

    private:
        iter iter_at(key_type key, value_type value) {
            return std::lower_bound(contents->begin(), contents->end(),
                        m_pair(key, value));
        }
};
#endif
