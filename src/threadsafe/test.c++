#include "include/atomic_ops.h"
#include <stdlib.h>
#include <iostream>
//#define TRIE true
#define atomic (volatile AO_t *)

class woop {
    public:
        int hascake;
        explicit woop() {
            hascake = 3;
        }
        int test() {
            return AO_load(atomic &hascake);
        }
};

int main() {
    woop *w = new woop;
    std::cout << w->test() << std::endl;

    int *cake = new int;
    *cake = 0;
    int test = AO_fetch_and_add1(atomic cake);
    std::cout << test << std::endl;
    test = AO_load(atomic cake);
    std::cout << test << std::endl;
    return 0;
}
