/*
By Jacob Wejendorp 
*/
#include "../threadsafe/ts_bursttrie.c++"
#include "../include/atomic_ops.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <stdlib.h>
#include <pthread.h> // Threading
#define NUM_THREADS 128
#define atomic (volatile AO_t *)


class crewVector {
    private:
        int current;
    public:
        std::vector<std::string> *v;
        crewVector() {
            current = 0;
            v = new std::vector<std::string>();
        }
        ~crewVector() {
            delete(v);
        }
        void reset() {
            current = 0;
        }
        int getNext() {
            int n = AO_fetch_and_add1(atomic &current);
            if(n < v->size())
                return n;
            return -1;
        }
        std::string get(int n) {
            if(n < v->size())
                return (*v)[n];
            return "";
        }
};
typedef ts_bursttrie<ts_node<std::string,
                         std::string*,
                         ts_array_bucket<std::string, std::string*>
                        >
                > trie;

struct thread_data{
    crewVector *v;
    trie *t;
} data;
//struct thread_data thread_data_array[NUM_THREADS];

void *thread_routine(void *input) {
    struct thread_data *d = (struct thread_data*)input;
    crewVector *v = d->v;
    trie *t = d->t;
    int n;
    while((n = v->getNext()) != -1) {
        std::string out = v->get(n);
        t->insert(out, NULL);
        //std::cout << std::endl ;
    }
    pthread_exit(NULL);
}

void *thread_reader_routine(void *input) {
    struct thread_data *d = (struct thread_data*)input;
    crewVector *v = d->v;
    trie *t = d->t;
    int n;
    while((n = v->getNext()) != -1) {
        std::string out = v->get(n);
        std::cout << t->find(out);
        //std::cout << std::endl ;
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t thread[NUM_THREADS];

    std::ifstream fin("../datasets/shakespeare.txt");
    std::string word;
    //std::vector<std::string> *data = new std::vector<std::string>();
    data.v = new crewVector();
    data.t = new trie();

    while(fin.good()) {
        fin >> word;
        data.v->v->push_back(word);
    }

   // std::vector<std::string>::iterator it;
   // for(it=data->begin(); it != data->end(); it++) {
   //     std::cout << (*it);
   // }

    // Insert the elements in parallel
    //
    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&thread[i], NULL, thread_routine, (void*)&data);
    }
    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_join(thread[i], NULL);
    }
    data.v->reset();
    // Retrieve the elements in parallel
    //
    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&thread[i], NULL, thread_reader_routine, (void*)&data);
    }
    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_join(thread[i], NULL);
    }

    delete(data.v);
    delete(data.t);
    return 0;
}


