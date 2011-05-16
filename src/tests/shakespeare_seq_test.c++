/*
By Jacob Wejendorp 
*/
#include "../seq/seq_bursttrie.c++"
#include "../seq/seq_sorted_bucket.c++"
#include "../seq/seq_unsorted_bucket.c++"
#include "../seq/seq_node.c++"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <stdlib.h>
#include <pthread.h> // Threading
#define NUM_THREADS 128
#define atomic (volatile AO_t *)

typedef unsigned long long int uint64_t;

typedef seq_bursttrie<
            seq_node<std::string, void*, seq_unsorted_bucket<
                                            std::string, void*>
                    >
            > btrie;
typedef std::vector<std::string> strVector;

int main(int argc, char *argv[]) {

    std::ifstream fin("../datasets/shakespeare.txt");
    std::string word;
    //std::vector<std::string> *data = new std::vector<std::string>();
    strVector *v = new strVector();
    btrie *t = new btrie();
    int i  = 0;

    while(fin.good()) {
        fin >> word;
        v->push_back(word);
        t->insert(word, (void*)i);
        i++;
    }

    strVector::iterator it;
    for(it=v->begin(); it != v->end(); it++) {
        std::string w = (*it);
        //std::cout << (uint64_t)t->find(w) << std::endl;
        t->find(w);
    }

    delete(v);
    delete(t);
    return 0;
}


