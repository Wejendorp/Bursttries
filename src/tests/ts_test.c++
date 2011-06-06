int *BUCKET_COUNT = new int(0);
int *BUCKETS_DESTROYED = new int(0);
int *NODE_COUNT = new int(0);
#include "../ts/ts_bursttrie.c++"

#include "crewVector.c++"
#include "test_thread.c++"

#include <map>
#include <string>
#include <stdlib.h>
#include <cstdio> //printf
#include <gsl/gsl_rng.h> // random number generator
#include <time.h>
#include <iostream>
#include <fstream>

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
        s->push_back(alphanum[u]);
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

typedef ts_bursttrie<std::string, std::string*, BUCKETTYPE, NODETYPE> testStruct;
typedef crewVector<std::string*> strVector;

int main() {
    test_thread<strVector, testStruct>* threads[NUM_THREADS];
    strVector  *v = new strVector();
    testStruct *t = NULL;
    testStruct **tp = &t;
    for(int i = 0; i < NUM_THREADS; i++) {
        threads[i] = new test_thread<strVector, testStruct>(v, tp);
    }

    std::cout<<std::endl<<"Starting test:"<<std::endl;
    SHOW_DEFINE(DATASET);
    SHOW_DEFINE(TESTSIZE);
    SHOW_DEFINE(ITERATIONS);
    SHOW_DEFINE(NUM_THREADS);
    SHOW_DEFINE(NODETYPE);
    SHOW_DEFINE(BUCKETTYPE);
    SHOW_DEFINE(BUCKETSIZE);

#ifndef DATASET
    // Generate random numbers 
    const gsl_rng_type * T;
    gsl_rng * r;
    gsl_rng_env_setup();

    T = gsl_rng_default;
    r = gsl_rng_alloc(T);

    for(int i = 0; i < TESTSIZE; i++) {
        std::string *st = new std::string();
        gen_random(st, r);
        v->v->push_back( st );
    }
    gsl_rng_free(r);
#else
    std::ifstream fin("./datasets/shakespeare.txt");
    std::string word;
    while(fin.good()) {
        fin >> word;
        std::string *st = new std::string(word);
        v->v->push_back(st);
    }
#endif

    // Start timer
    //
    timespec start = startTimer();
    for(int i = 0; i < ITERATIONS; i++) {
        v->reset();
        delete(t);
        t = new testStruct();
        *BUCKET_COUNT = 0;
        *BUCKETS_DESTROYED = 0;
        *NODE_COUNT = 0;
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
    std::cout << "Created "<<*NODE_COUNT << " nodes!"<<std::endl;
    std::cout << "Created "<<*BUCKET_COUNT << " buckets!"<<std::endl;
    std::cout << "Destroyed "<<*BUCKETS_DESTROYED << " buckets!"<<std::endl;


    start = startTimer();
    // Search in datastructure
    for(int j = 0; j < ITERATIONS; j++) {
        v->reset();
        // Create writer-threads
        for(int j = 0; j < NUM_THREADS; j++) {
            threads[j]->read(false); // disable strict
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
    for(unsigned int i = 0; i < v->v->size(); i++) {
       delete((*v->v)[i]);
    }
    for(int i = 0; i < NUM_THREADS; i++) {
       delete( threads[i] ) ;
    }
    delete(v);
    delete(t);
    delete(BUCKET_COUNT);
    delete(BUCKETS_DESTROYED);
    delete(NODE_COUNT);

    return 0;
}
