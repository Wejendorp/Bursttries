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

    public:

        ts_bucket_iterator(B *b, int start_index) {
            index = new int;
            *index = start_index;
            bucket = b;
        }
        bool operator<(ts_bucket_iterator other) {
            return (AO_load(this->index) < AO_load_dd_acquire_read(other->index));
        }
        T *operator*() {
            return AO_load(&(bucket->contents[AO_load_dd_acquire_read(index)]));
        }
        ts_bucket_iterator &operator++() {
            AO_fetch_and_add1(index);
            return *this;
        }
}
