//#include "btree_trie.c++"
//#include "basic_array_trie.c++"
#include "bursttrie_array.c++"
#include <map>
#include <string>
#include <stdlib.h>

//#define TRIE true

void gen_random(char *s, const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    s[len-1] = 0;
}

int main() {
    trie<std::string> t;
    std::map<std::string, std::string*> m;
    std::vector<std::string*> keys(TESTSIZE);
    srand ( SEED );
    int len = 10; // (rand() % 12) + 1;
    char s[10];
    for(int i = 0; i < TESTSIZE; i++) {
        gen_random(s, len);
        std::string *st = new std::string(s);
        if(TRIE)
            t.insert(*st, st);
        else
            m[*st] = st;
        keys[i] = st;
    }

    for(int i = 0; i < TESTSIZE; i++) {
       if(SEARCH) {
           if (TRIE && keys[i] != t.search(*(keys[i])))
               std::cout << "Mismatched search!\n";
           else
               m.find(*(keys[i]));
       }
       delete(keys[i]);
    }
    return 0;
}
