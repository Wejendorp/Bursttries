#include "../include/atomic_ops.h"
#include <iterator>
#define atomic (volatile AO_t *)

template<
    typename B
>
class ts_bucket_iterator {
    private:
        volatile int *index;
        B *bucket;

        typedef typename B::valuetype T;

    public:

        ts_bucket_iterator(B *b, int start_index) {
            index = new int;
            *index = start_index;
            bucket = b;
        }
        bool operator<(ts_bucket_iterator other) {
            return (AO_load(atomic index) < AO_load_dd_acquire_read(atomic other->index));
        }
        T *operator*() {
            return AO_load(atomic &(bucket->contents[AO_load_dd_acquire_read(atomic index)]));
        }
        ts_bucket_iterator &operator++() {
            AO_fetch_and_add1(atomic index);
            return *this;
        }
};
