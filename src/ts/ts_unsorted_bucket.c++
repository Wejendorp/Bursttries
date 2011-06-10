
#ifndef __TS_UNSORTED_BUCKET
#define __TS_UNSORTED_BUCKET

#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <string.h>
#define m_pair std::make_pair<key_type, value_type>

template<
    typename N
>
class ts_unsorted_bucket {
    public:
        typedef typename N::value_type value_type;
        typedef typename N::key_type key_type;
        typedef N node;
        typedef ts_unsorted_bucket<N> bucket;

        typedef std::pair<key_type, value_type> pair;
        typedef std::vector<pair > pairvector;
        typedef std::allocator<pairvector> vector_alloc_t;
        vector_alloc_t vector_allocator;

        typedef typename pairvector::iterator iterator;
        unsigned int capacity;

        pairvector *contents;
        pthread_rwlock_t lock;
        bucket *left, *right;

        explicit ts_unsorted_bucket(int cap) {
            contents = vector_allocator.allocate(1);
            vector_allocator.construct(contents, pairvector(0));
            capacity = cap;
            left = NULL;
            right = NULL;
            pthread_rwlock_init(&lock, NULL);
            AO_fetch_and_add1(atomic BUCKET_COUNT);
        }
        ~ts_unsorted_bucket() {
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

        void insert(const key_type &key, const value_type &value, pthread_rwlock_t *oldLock = NULL) {
            pthread_rwlock_wrlock(&lock);
            if(oldLock) pthread_rwlock_unlock(oldLock);

            contents->push_back(m_pair(key, value));
            pthread_rwlock_unlock(&lock);
        }
        bool remove(const key_type &key, pthread_rwlock_t *oldLock = NULL) {
            pthread_rwlock_wrlock(&lock);
            if(oldLock) pthread_rwlock_unlock(oldLock);
            bool ret = false;
            iterator it;
            for(it = contents->begin(); it != contents->end(); it++) {
                if((*it).first == key){
                    contents->erase(it);
                    ret = true;
                    break;
                }
            }
            pthread_rwlock_unlock(&lock);
            return ret;
        }
        value_type find(const key_type &key, pthread_rwlock_t *oldLock = NULL) {
            pthread_rwlock_rdlock(&lock);
            if(oldLock) pthread_rwlock_unlock(oldLock);
            value_type ret = NULL;
            iterator it;
            for(it = contents->begin(); it != contents->end(); it++) {
                if((*it).first == key) {
                    ret = (*it).second;
                    break;
                }
            }
            pthread_rwlock_unlock(&lock);
            return ret;
        }
        node *burst() {
            node *newnode = NULL;
            pthread_rwlock_wrlock(&lock);
            if(contents->size() > capacity) {
                newnode = new node();
                iterator it;
                for(it = contents->begin(); it != contents->end(); it++) {
                    pair p = (*it);
                    newnode->insert(p.first, p.second);
                }
            }
            pthread_rwlock_unlock(&lock);
            return newnode;
        }
        iterator begin() {
            return contents->begin();
        }
        iterator end() {
            return contents->end();
        }
};
#endif
