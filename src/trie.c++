/*   Basic trie implementation
 *   for bachelorprojekt "Implementing and parallelising the burst trie"
 *   By Jacob Wejendorp 2012
 * */

#include <iostream>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#define NDEBUG // uncomment to disable asserts

#define SIZE 128
#define ROOT true
#define NOROOT false

#define TESTSIZE 100000

template<
    typename V
//    typename K = std::string,
//    typename A = std::allocator<V>
>
class trie {
    private:
        struct node_t {
            node_t *children[SIZE]; // Children array
            node_t *parent; // Parent pointer, for deletion
            int parentindex;// Knows its index in the parent, for deletions
            int childcount; // Keep a counter of children, for deletions
            V *value; // Value pointer
        } root_node;

        typedef std::string K;
        typedef std::allocator<V> A;
        A allocator;
        typedef typename A::template rebind<node_t>::other node_alloc_t;
        node_alloc_t node_allocator;


        node_t *create_node(node_t *parent, int index) {
            node_t *new_node = node_allocator.allocate(1);
            memset(new_node->children, 0, SIZE*sizeof(node_t*));
            if(parent != NULL) {
                new_node->parentindex = index;
                parent->children[index] = new_node;
                parent->childcount++;
            }
            new_node->parent = parent;
            new_node->value = NULL;
            new_node->childcount = 0;

            return new_node;
        }
        node_t *delete_node(node_t *node, bool root = NOROOT) {
            if(node == NULL) return NULL;
            if(root)
                for(int i = 0; i < SIZE; i++) {
                    delete_node(node->children[i], root);
                }
            if(node != &root_node) {
                node_t *parent = node->parent;
                node->parent = NULL;
                node->value = NULL;
                parent->children[node->parentindex] = NULL;
                parent->childcount--;
                node_allocator.deallocate(node, 1);
                return parent;
            }
            return NULL;
        }
        node_t *find_node(K key) {
            node_t *node = &root_node;
            int i = 0;
            while(key[i] != '\0') {
                if(node->children[key[i]] != NULL)
                    node = node->children[key[i]];
                else
                    return NULL;
                i++;
            }
            return node;
        }
    public:
        explicit trie(A const& a = A()) : allocator(a) {
            memset(root_node.children, 0, SIZE*sizeof(node_t*));
            root_node.childcount = 0;
            root_node.value = NULL;
            root_node.parent = NULL;
        }

        ~trie() {
            delete_node(&root_node, ROOT);
        }

        V *search(K key) {
            node_t *node = find_node(key);
            return node != NULL ? node->value : NULL;
        }

        void insert(K key, V *value) {
            node_t *node = &root_node;
            int i = 0;
            while(key[i] != '\0') {
                if(node->children[key[i]] != NULL)
                    node = node->children[key[i]];
                else
                    node = create_node(node, key[i]);
                i++;
            }
            if(!node->value) node->value = value;
        }

        void erase(K key) {
            node_t *node = find_node(key);
            if(node == NULL) return;
            if(node->childcount != 0)
                node->value = NULL;
            else
                while(node->childcount == 0 && node != &root_node)
                    node = delete_node(node);
        }
};

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
    std::vector<std::string*> keys(TESTSIZE);

    int len = 5; // (rand() % 12) + 1;
    char *s = new char[len];
    for(int i = 0; i < TESTSIZE; i++) {
        gen_random(s, len);
        std::string *st = new std::string(s);
        t.insert(*st, st);
        keys[i] = st;
    }
    delete [] s;

    for(int i = 0; i < TESTSIZE; i++) {
        if(keys[i] != t.search(*(keys[i])))
            std::cout << "Mismatched search!\n";
        delete(keys[i]);
    }
    return 0;
}

