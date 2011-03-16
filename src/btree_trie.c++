/*   Basic trie implementation
 *   - Attempting binary tree children
 *   for bachelorprojekt "Implementing and parallelising the burst trie"
 *   By Jacob Wejendorp 2012
 *
 *   Insertions, searches and deletions in O(l).
 *   But has a huge memory waste, from allocation of 128 pointers in
 *   each node.
 * */

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <string.h>
#include "btree.c++"

#define NODESIZE 128

template<
    typename V,
    typename K = std::string
>
class trie {
    private:
        struct node_t {
            btree<node_t> *children; // Children array
            node_t *parent; // Parent pointer, for deletion
            int parentindex;// Knows its index in the parent, for deletions
            int childcount; // Keep a counter of children, for deletions
            V *value; // Value pointer
        } root_node;
        int nodes_created;

        typedef std::allocator<node_t> node_alloc_t;
        node_alloc_t node_allocator;

        node_t *create_node(node_t *parent, int index) {
            nodes_created++;
            node_t *new_node = node_allocator.allocate(1);
            new_node->children = new btree<node_t>();
            parent->children->set(index, new_node);
            parent->childcount++;
            new_node->parentindex = index;
            new_node->parent = parent;
            new_node->value = NULL;
            new_node->childcount = 0;

            return new_node;
        }
        node_t *delete_node(node_t *node) {
            if(node == NULL) return NULL;
            for(int i = 0; node->childcount > 0, i < NODESIZE; i++)
                delete_node(node->children->search(i));
            delete(node->children);
            if(node != &root_node) {
                node_t *parent = node->parent;
                node->parent = NULL;
                node->value = NULL;
                parent->children->set(node->parentindex, NULL);
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
                if(node->children->search(key[i]) != NULL)
                    node = node->children->search(key[i]);
                else
                    return NULL;
                i++;
            }
            return node;
        }
    public:
        explicit trie() {
            root_node.children = new btree<node_t>();
            root_node.childcount = 0;
            root_node.value = NULL;
            root_node.parent = NULL;
            nodes_created = 0;
        }

        ~trie() {
            delete_node(&root_node);
            std::cout << nodes_created << " nodes were made\n";
        }

        V *search(K key) {
            node_t *node = find_node(key);
            return node != NULL ? node->value : NULL;
        }

        void insert(K key, V *value) {
            node_t *node = &root_node;
            int i = 0;
            while(key[i] != '\0') {
                if(node->children->search(key[i]) != NULL)
                    node = node->children->search(key[i]);
                else
                    node = create_node(node, key[i]);
                i++;
            }
            if(!node->value) node->value = value; // No overwrite
        }

        void erase(K key) {
            node_t *node = find_node(key);
            if(node == NULL) return;
            if(node->childcount != 0)
                node->value = NULL;
            else
                while(node->childcount == 0 && node != &root_node \
                        && node->value == NULL)
                    node = delete_node(node);
        }
};
