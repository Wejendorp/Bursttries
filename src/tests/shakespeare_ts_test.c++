/*
By Jacob Wejendorp 
*/
#include "../ts/ts_bursttrie.c++"
#include "../ts/ts_locked_node.c++"
#include "../ts/ts_btree_bucket.c++"
#include "../include/atomic_ops.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <stdlib.h>
#include <pthread.h> // Threading
#define NUM_THREADS 1024
#define atomic (volatile AO_t *)

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
typedef ts_bursttrie<ts_locked_node<std::string,
                         std::string*,
                         ts_btree_bucket<std::string, std::string*>
                        >
                > trie;
typedef crewVector<std::string*> strVector;
typedef std::map<std::string, std::string*> map;

struct thread_data{
    strVector *v;
    trie *t;
    map *m;
} data;
//struct thread_data thread_data_array[NUM_THREADS];

void *thread_routine(void *input) {
    struct thread_data *d = (struct thread_data*)input;
    std::string *out;
    while((out = d->v->getNext()) != NULL) {
        d->t->insert(std::make_pair<std::string, std::string*>(*out, out));
    }
    pthread_exit(NULL);
}

void *thread_reader_routine(void *input) {
    struct thread_data *d = (struct thread_data*)input;
    std::string *out;
    while((out = d->v->getNext()) != NULL) {
        if(*out != *(d->t->find(*out)))
            std::cout << std::endl;
        // "Mismatched search for " << *out << std::endl;
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t thread[NUM_THREADS];

    std::ifstream fin("./datasets/shakespeare.txt");

    data.v = new strVector();
    data.t = new trie();
    data.m = new map();

    std::string word;
    while(fin.good()) {
        fin >> word;
        std::string *st = new std::string(word);
        data.v->v->push_back(st);
        (*data.m)[*st] = st;
    }

    // Insert the elements in parallel
    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&thread[i], NULL, thread_routine, (void*)&data);
    }
    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_join(thread[i], NULL);
    }
    data.v->reset();
    // Retrieve the elements in parallel
    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&thread[i], NULL, thread_reader_routine, (void*)&data);
    }
    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_join(thread[i], NULL);
    }

    delete(data.v);
    delete(data.t);
    delete(data.m);
    return 0;
}
