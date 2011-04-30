#include "include/atomic_ops.h"
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <string.h>
#define m_pair std::make_pair<K, V*>

#define BURSTING 1
#define IDLE 0
#define atomic (volatile AO_t *)

template<
    typename V,
    typename K
>
class ts_array_bucket {
    private:
        typedef std::vector<std::pair<K, V*> > pairvector
        typedef std::allocator<pairvector> vector_alloc_t;
        vector_alloc_t vector_allocator;

        typedef pairvector::iterator iter;
        typedef ts_array_bucket_iterator iterator;

        volatile int active;
        pairvector *contents;
        iterator burstiterator;

        friend class ts_array_bucket_iterator<ts_array_bucket<V,K> >;

    public:
        explicit ts_array_bucket() {
            contents = vector_allocator.allocate(1);
            vector_allocator.construct(contents, pairvector(0));
            burstiterator = NULL;
            AO_store(atomic &active, 0);
        }
        ~ts_array_bucket() {
            vector_allocator.destroy(contents);
            vector_allocator.deallocate(contents, 1);
        }
        void insert(K key, V value) {
            iter it;
            // If bursting, insert at end to avoid relocations
            if (state != BURSTING) {
                it = iter_at(key, value);
            } else {
                it = contents.end();
            }
            contents.insert(it, m_pair(key, value))
        }
        bool remove(K key) {
            iter it = iter_at(key, NULL);
            if((*it).first != key)
                return false;
            contents.erase(it);
            return true;
        }
        V *find(K key) {
            iter it = iter_at(key, NULL);
            if((*it).first == key) return (*it).second;
            return NULL;
        }
        ts_array_bucket_iterator burst() {
            state = BURSTING;
            return iterator(this, 0);
        }

    private:
        iter iter_at(K key, V *value) {
            return std::lower_bound(contents.begin(), contents.end(),
                        m_pair(key, value));
        }
}
