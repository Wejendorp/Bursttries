#include "../seq/seq_bursttrie.c++"
#include "../seq/seq_sorted_bucket.c++"
#include "../seq/seq_unsorted_bucket.c++"
#include "../seq/seq_btree_bucket.c++"
#include "../seq/seq_node.c++"
#include <map>
#include <string>
#include <stdlib.h>
#include <gsl/gsl_rng.h> // random number generator
#include <time.h>

#define ITERATIONS 1

// Generate a random string with a given alphabet,
// and place it in the given string
//
void gen_random(std::string *s, const int len, gsl_rng *r) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    for(int i = 0; i < len; i++) {
        int u = gsl_rng_uniform_int(r, sizeof(alphanum));
        s->replace(i, 1,1, alphanum[u]);
    }
}


// Timing function, calculate difference
// between start and stop times with nanosec precision
//
timespec diff(timespec start, timespec end)
{
    timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}
typedef seq_bursttrie<
            seq_node<std::string, std::string*, BUCKETTYPE<
                                            std::string, std::string*>
                    >
            > btrie;

int main() {
    btrie *t = new btrie();
    std::map<std::string, std::string*> m;
    std::vector<std::string*> keys(TESTSIZE);
    // Generate random numbers 
    const gsl_rng_type * T;
    gsl_rng * r;

    gsl_rng_env_setup();

    T = gsl_rng_default;
    r = gsl_rng_alloc(T);

    int len = 10; // (rand() % 12) + 1;
    char s[10];
    for(int i = 0; i < TESTSIZE; i++) {
        std::string *st = new std::string("aaaaaaaaaa");
        gen_random(st, len, r);
        keys[i] = st;
    }
    gsl_rng_free(r);

    // Start timer
    timespec start, stop, temp;
    temp.tv_sec = 0;
    temp.tv_nsec = 0;
    clock_gettime(CLOCK_REALTIME, &start);
    for(int j = 0; j < ITERATIONS; j++) {
        // Insert into datastructure
        for(int i = 0; i < TESTSIZE; i++) {
            std::string *st = keys[i];
            if(TRIE)
                t->insert(*st, st);
            else
                m[*st] = st;
            //std::cout << "inserting " << *st << " == " << st <<std::endl;
        }

    }
    clock_gettime(CLOCK_REALTIME, &stop);
    temp.tv_sec += diff(start, stop).tv_sec;
    temp.tv_nsec += diff(start, stop).tv_nsec;
    std::cout<<"Inserted "<<TESTSIZE<<" string:pointer pairs in an average of ";
    std::cout <<temp.tv_sec/ITERATIONS<<":"<<temp.tv_nsec/ITERATIONS<<std::endl;

    clock_gettime(CLOCK_REALTIME, &start);
    // Search in datastructure
    for(int i = 0; i < TESTSIZE; i++) {
       //if(SEARCH) {
            if (TRIE && keys[i] != t->find(*(keys[i]))) {
               std::cout << "Mismatched search for "<< *(keys[i])<<std::endl;
               std::cout << "Expected " << keys[i] << std::endl;
               std::cout << "got " << t->find(*(keys[i])) << std::endl;
            } else if(!TRIE && keys[i] != m.find(*(keys[i]))->second) {
                std::cout << "Fix the test!";
            }
       //}
    }
    clock_gettime(CLOCK_REALTIME, &stop);
    std::cout<<"Searched "<<TESTSIZE<<" string:pointer pairs in ";
    std::cout <<diff(start,stop).tv_sec<<":"<<diff(start,stop).tv_nsec<<std::endl;
    for(int i = 0; i < TESTSIZE; i++) {
       delete(keys[i]);
    }
    return 0;
}
