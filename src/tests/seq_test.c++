int *BUCKET_COUNT = new int(0);
int *BUCKETS_DESTROYED = new int(0);
int *NODE_COUNT = new int(0);
#ifdef TRIE
#include "../include/atomic_ops.h"
#define atomic (volatile AO_t*)
#include "../seq/seq_bursttrie.c++"
#include "../seq/seq_sorted_bucket.c++"
#include "../seq/seq_unsorted_bucket.c++"
#include "../seq/seq_btree_bucket.c++"
#include "../seq/seq_map_bucket.c++"
#include "../seq/seq_node.c++"
#endif

#include <map>
#include <string>
#include <stdlib.h>
#include <cstdio> //printf
#include <gsl/gsl_rng.h> // random number generator
#include <time.h>
#include <iostream>
#include <vector>

#ifndef BUCKETTYPE
#define BUCKETTYPE map
#endif
#define STR(x)   #x
#define SHOW_DEFINE(x) std::printf("%s=%s\n", #x, STR(x))

// Generate a random string with a given alphabet,
// and place it in the given string
//
void gen_random(std::string *s, gsl_rng *r) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    int len = gsl_rng_uniform_int(r, 7)+5;
    for(int i = 0; i < len; i++) {
        int u = gsl_rng_uniform_int(r, sizeof(alphanum));
        s->replace(i, 1,1, alphanum[u]);
    }
}


// timing function, calculate difference
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
timespec startTimer() {
    timespec start;
    clock_gettime(CLOCK_REALTIME, &start);
    return start;
}
void stopTimer(timespec start) {
    timespec stop;
    clock_gettime(CLOCK_REALTIME, &stop);
    long long int nsec = (diff(start, stop).tv_sec*1000000000 + diff(start, stop).tv_nsec)/(ITERATIONS);
    long long int sec = nsec/1000000000;
    std::printf("%lld:%09lld\n", sec, nsec % 1000000000);

}
#ifdef TRIE
    typedef seq_bursttrie<std::string, std::string*, BUCKETTYPE, seq_node> testStruct;
#else
    typedef std::map<std::string, std::string *> testStruct;
#endif

int main() {
    testStruct *t = NULL;
    std::vector<std::string*> keys(TESTSIZE);

    std::cout<<std::endl<<"Starting test:"<<std::endl;
    SHOW_DEFINE(TESTSIZE);
    SHOW_DEFINE(ITERATIONS);
    SHOW_DEFINE(NODETYPE);
    SHOW_DEFINE(BUCKETTYPE);
    SHOW_DEFINE(BUCKETSIZE);

    // Generate random numbers 
    const gsl_rng_type * T;
    gsl_rng * r;
    gsl_rng_env_setup();

    T = gsl_rng_default;
    r = gsl_rng_alloc(T);

    for(int i = 0; i < TESTSIZE; i++) {
        std::string *st = new std::string();
        gen_random(st,r);
        keys[i] = st;
    }
    gsl_rng_free(r);

    // Start timer
    timespec start = startTimer();
    for(int j = 0; j < ITERATIONS; j++) {
        // Insert into datastructure
        if(t != NULL) delete(t);
        t = new testStruct();

        for(int i = 0; i < TESTSIZE; i++) {
            std::string *st = keys[i];
            t->insert(std::make_pair<std::string, std::string*>(*st, st));
            //std::cout << "inserting " << *st << " == " << st <<std::endl;
        }

    }
    std::cout << "Inserted " << TESTSIZE << " entries in an average of ";
    stopTimer(start);
    std::cout << "Created "<<*NODE_COUNT << " nodes!"<<std::endl;
    std::cout << "Created "<<*BUCKET_COUNT << " buckets!"<<std::endl;
    std::cout << "Destroyed "<<*BUCKETS_DESTROYED << " buckets!"<<std::endl;

    start = startTimer();
    // Search in datastructure
    for(int j = 0; j < ITERATIONS; j++) {
        for(int i = 0; i < TESTSIZE; i++) {
#ifdef TRIE
            std::string *res = t->find(*(keys[i]));
#else
            std::string * res = (*t->find(*(keys[i]))).second;
#endif
            if (keys[i] != res) {
                std::cout << "Mismatched search for "<< *(keys[i])<<std::endl;
                //std::cout << "Expected " << keys[i] << std::endl;
                //std::cout << "got " << t->find(*(keys[i])) << std::endl;
            }
        }
    }
    std::cout<<"Searched "<<TESTSIZE<<" entires in an average of ";
    stopTimer(start);

    for(int i = 0; i < TESTSIZE; i++) {
       delete(keys[i]);
    }
    delete(t);
    return 0;
}
