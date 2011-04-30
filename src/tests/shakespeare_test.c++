/*
By Jacob Wejendorp 
*/
#include "include/atomic_ops.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <stdlib.h>
#include <pthread.h> // Threading
#define THREADS 128
#define atomic (volatile AO_t *)

class sexiVector {
    private:
        int current;
    public:
        std::vector<std::string> *v;
        sexiVector() {
            current = 0;
            v = new std::vector<std::string>();
        }
        ~sexiVector() {
            delete(v);
        }
        std::string getNext() {
            int n = AO_fetch_and_add1(atomic &current);
            if(n < v->size())
                return (*v)[n];
            return "";
        }
};
void *thread_routine(void *i) {
    sexiVector *v = (sexiVector*)i;
    while(true) {
        std::string out = v->getNext();
        if (out == "") return NULL;
 //       std::cout << std::endl ;
    }
}

int main(int argc, char *argv[]) {
    pthread_t thread[THREADS];

    std::ifstream fin("datasets/shakespeare.txt");
    std::string word;
    //std::vector<std::string> *data = new std::vector<std::string>();
    sexiVector *v = new sexiVector();
    while(fin.good()) {
        fin >> word;
        v->v->push_back(word);
    }

   // std::vector<std::string>::iterator it;
   // for(it=data->begin(); it != data->end(); it++) {
   //     std::cout << (*it);
   // }
    for(int i = 0; i < THREADS; i++) {
        pthread_create(&thread[i], NULL, thread_routine, (void*)v);
    }

    for(int i = 0; i < THREADS; i++) {
        pthread_join(thread[i], NULL);
    }
    delete(v);
    return 0;
}


