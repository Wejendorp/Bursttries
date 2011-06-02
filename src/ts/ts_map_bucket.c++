#ifndef __TS_MAP_BUCKET
#define __TS_MAP_BUCKET

#include <map>
template<
    typename N
>
class ts_map_bucket {
    public:
        typedef typename N::value_type value_type;
        typedef typename N::key_type key_type;
        typedef N node;
        typedef std::map<key_type,value_type> map;
        map *m;
        volatile unsigned int capacity, size;
        pthread_rwlock_t rwlock;

        typedef typename map::iterator iterator;

        explicit ts_map_bucket(int cap) {
            capacity = cap;
            size = 0;
            m = new map();
            pthread_rwlock_init(&rwlock, NULL);
            AO_fetch_and_add1(atomic BUCKET_COUNT);
        }
        ~ts_map_bucket() {
            AO_fetch_and_add1(atomic BUCKETS_DESTROYED);
            pthread_rwlock_destroy(&rwlock);
            delete(m);
        }
        void insert(const key_type &k, const value_type &v, pthread_rwlock_t *oldLock = NULL) {
            pthread_rwlock_wrlock(&rwlock);
            if(oldLock) pthread_rwlock_unlock(oldLock);
            size++;
            (*m)[k] = v;
            pthread_rwlock_unlock(&rwlock);
        }
        bool remove(const key_type &key, pthread_rwlock_t *oldLock = NULL) {
            bool ret = NULL;
            pthread_rwlock_wrlock(&rwlock);
            if(oldLock) pthread_rwlock_unlock(oldLock);
            ret = (bool)m->erase(key);
            if(ret) size--;
            pthread_rwlock_unlock(&rwlock);
            return ret;
        }
        value_type find(const key_type &key, pthread_rwlock_t *oldLock = NULL) {
            iterator it;
            value_type ret = NULL;
            pthread_rwlock_rdlock(&rwlock);
            if(oldLock) pthread_rwlock_unlock(oldLock);
            it = m->find(key);
            if(it != m->end()) ret = it->second;
            pthread_rwlock_unlock(&rwlock);
            return ret;
        }
        node *burst() {
            pthread_rwlock_wrlock(&rwlock);
            node *newnode = NULL;
            if(size > capacity) {
                size = 0;
                newnode = new node();
                typename map::iterator it;
                for(it = m->begin(); it != m->end(); it++) {
                    newnode->insert(*it);
                }
            }
            pthread_rwlock_unlock(&rwlock);
            return newnode;
        }

        iterator begin() {
            return m->begin();
        }
        iterator end() {
            return m->end();
        }
};
#endif
