#ifdef TRIE
#include "../ts/ts_bursttrie.c++"
#include "../ts/ts_sorted_bucket.c++"
#include "../ts/ts_unsorted_bucket.c++"
#include "../ts/ts_btree_bucket.c++"
#include "../ts/ts_map_bucket.c++"
#include "../ts/ts_locked_node_2.c++"
#endif

#include "crewVector.c++"
#include "test_thread.c++"

#include <map>
#include <string>
#include <stdlib.h>
#include <cstdio> //printf
#include <gsl/gsl_rng.h> // random number generator
#include <time.h>
#include <iostream>

#define ITERATIONS 10
#ifndef BUCKETTYPE
#define BUCKETTYPE map
#endif
#define STR(x)   #x
#define SHOW_DEFINE(x) std::printf("%s=%s\n", #x, STR(x))

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
timespec startTimer() {
    timespec start;
    clock_gettime(CLOCK_REALTIME, &start);
    return start;
}
void stopTimer(timespec start) {
    timespec stop;
    clock_gettime(CLOCK_REALTIME, &stop);
    long long int nsec = (diff(start, stop).tv_sec*1000000000 + diff(start, stop).tv_nsec)/ITERATIONS;
    long long int sec = nsec/1000000000;
    std::printf("%lld:%09lld\n", sec, nsec % 1000000000);

}
#ifdef TRIE
    typedef ts_bursttrie<
                ts_locked_node<std::string, std::string*, BUCKETTYPE<
                                                std::string, std::string*>
                        >
                > testStruct;
#else
    typedef std::map<std::string, std::string *> testStruct;
#endif
typedef crewVector<std::string*> strVector;

int main() {
    test_thread<strVector, testStruct>* threads[NUM_THREADS];
    strVector  *v = new strVector();
    testStruct *t = new testStruct();
    for(int i = 0; i < NUM_THREADS; i++) {
        threads[i] = new test_thread<strVector, testStruct>(v, t);
    }

    std::cout<<std::endl<<"Starting test:"<<std::endl;
    SHOW_DEFINE(BUCKETTYPE);
    SHOW_DEFINE(BUCKETSIZE);
    SHOW_DEFINE(NUM_THREADS);
    SHOW_DEFINE(ITERATIONS);

    // Generate random numbers 
    const gsl_rng_type * T;
    gsl_rng * r;
    gsl_rng_env_setup();

    T = gsl_rng_default;
    r = gsl_rng_alloc(T);

    int len = 10; // (rand() % 12) + 1;
    for(int i = 0; i < TESTSIZE; i++) {
        std::string *st = new std::string("aaaaaaaaaa");
        gen_random(st, len, r);
        v->v->push_back( st );
    }
    gsl_rng_free(r);
    // Start timer
    //
    timespec start = startTimer();
    for(int i = 0; i < ITERATIONS; i++) {
        v->reset();
        // Create writer-threads
        for(int j = 0; j < NUM_THREADS; j++) {
            threads[j]->write();
        }
        // Join threads
        for(int j = 0; j < NUM_THREADS; j++) {
            threads[j]->join();
        }
    }
    std::cout << "Inserted " << TESTSIZE << " entries in an average of ";
    stopTimer(start);


    start = startTimer();
    // Search in datastructure
    for(int j = 0; j < ITERATIONS; j++) {
        v->reset();
        // Create writer-threads
        for(int j = 0; j < NUM_THREADS; j++) {
            threads[j]->read();
        }
        // Join threads
        for(int j = 0; j < NUM_THREADS; j++) {
            threads[j]->join();
        }

    }
    std::cout<<"Searched "<<TESTSIZE<<" entires in an average of ";
    stopTimer(start);


    // Cleanup!!
    //
    for(int i = 0; i < TESTSIZE; i++) {
       delete((*v->v)[i]);
    }
    for(int i = 0; i < NUM_THREADS; i++) {
        threads[i] = new test_thread<strVector, testStruct>(v, t);
    }
    delete(v);
    delete(t);
    return 0;
}
