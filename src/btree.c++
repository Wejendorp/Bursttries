/*
 * Binary tree vector implementation
 *
 * */
#include <vector>
#include <math.h>
#include <string.h>
#include <limits.h>
#include <utility>
#include <iostream>

#define TREE_UNDEF INT_MAX
#define DEFVAL std::make_pair(0,(V*)TREE_UNDEF)
template<
    typename V
>
class btree {
    private:
        std::vector<std::pair<int,V*> > *vector;
    public:
        explicit btree() {
            vector = new std::vector<std::pair<int,V*> >(1, DEFVAL);
        }
        ~btree() {
            delete(vector);
        }
        void set(int key, V *value) {
            int i = 0, sz = vector->size();
            while(i < sz && ((*vector)[i]).second != (V*)TREE_UNDEF) {
                if(((*vector)[i]).first == key) break;
                if(((*vector)[i]).first < key) {
                    i = 2*i+1;
                } else {
                    i = 2*i+2;
                }
            }
            if(i >= sz) vector->resize(sz*4, DEFVAL);
            ((*vector)[i]) = std::make_pair(key, value);
        }
        V *search(int key) {
            int i = 0;
            while(i < vector->size()) {
                if(((*vector)[i]).first == key && ((*vector)[i]).second != (V*)TREE_UNDEF ) return ((*vector)[i]).second;
                if(((*vector)[i]).first < key) {
                    i = 2*i+1;
                } else {
                    i = 2*i+2;
                }
            }
            return NULL;
        }
        void remove(int key) {
            int i = 0;
            while(i < vector->size()) {
                if(((*vector)[i]).first == key) {
                    ((*vector)[i]) = DEFVAL;
                    return;
                }
                if(((*vector)[i]).first < key) {
                    i = 2*i+1;
                } else {
                    i = 2*i+2;
                }
            }
        }
};
