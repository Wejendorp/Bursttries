#include "../include/atomic_ops.h"
#define atomic (volatile AO_t *)
#include <vector>
template<
    typename T
>
class crewVector {
    private:
        int current;
    public:
        std::vector<T> *v;
        crewVector() {
            current = 0;
            v = new std::vector<T>();
        }
        ~crewVector() {
            delete(v);
        }
        void reset() {
            current = 0;
        }
        T getNext() {
            volatile int n = AO_fetch_and_add1(atomic &current);
            if(n < v->size())
                return (*v)[n];
            return NULL;
        }
};
