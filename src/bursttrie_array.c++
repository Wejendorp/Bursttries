/*   Burst trie implementation
 *   for bachelorprojekt "Implementing and parallelising the burst trie"
 *   By Jacob Wejendorp 2012
 *
 *   Insertions, searches and deletions in O(l).
 *   But has a huge memory waste, from allocation of 128 pointers in
 *   each node.
 * */

#include <iostream>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

#define NODESIZE 128
#define BUCKETCAPACITY 256

#define UNUSED 0
#define BUCKET 1
#define NODE 2

template<
    typename V,
    typename K = std::string
>
class trie {
    private:
        struct bucket_t {
            std::vector<std::pair<K, V*> > contents;
        };
        struct child;
        struct node_t {
            child children[NODESIZE]; // Children array
        };
        struct child {
            //enum {UNUSED = 0, BUCKET = 1, NODE = 2 } tag;
            char tag;
            union {
                node_t   *n;
                bucket_t *b;
            };
        } root;
        node_t root_node;
        int nodes_created;
        int buckets_created;

        typedef std::allocator<node_t> node_alloc_t;
        node_alloc_t node_allocator;
        typedef std::allocator<std::vector<std::pair<K, V*> > > vector_alloc_t;
        vector_alloc_t vector_allocator;
        typedef std::allocator<bucket_t> bucket_alloc_t;
        bucket_alloc_t bucket_allocator;
        typedef std::allocator<child> child_alloc_t;
        child_alloc_t child_allocator;

        /*
         * Constructors and delete
         * */
        node_t *create_node(char index) {
            nodes_created++;
            node_t *new_node = node_allocator.allocate(1);
            memset(new_node->children, 0, sizeof(new_node->children));

            return new_node;
        }
        void delete_node(child *node) {
            if(node == NULL || node->tag != NODE) return;
            node->tag = UNUSED;
            child *p;
            for(int i = 0; i < NODESIZE; i++) {
                p = &node->n->children[i];
                if(p->tag == NODE)
                    delete_node(p);
                else if(p->tag == BUCKET) {
                    delete_bucket(p->b);
              //      child_allocator.deallocate(p, 1);
                }
            }
            if(node != &root) {
                node_allocator.deallocate(node->n, 1);
                node->n = NULL;
            }
        }
        bucket_t *create_bucket() {
            buckets_created++;
            bucket_t * b = bucket_allocator.allocate(1);
            vector_allocator.construct(&b->contents, std::vector<std::pair<K, V*> >(0));
            return b;
        }
        void delete_bucket(bucket_t *b) {
            vector_allocator.destroy(&b->contents);
            vector_allocator.deallocate(&b->contents, 1);
        }

        /*
         * Searches
         * */
        std::pair<K, bucket_t *> find_bucket(K key) {
            node_t *node = &root_node;
            int i = 0;
            while(key[i] != '\0') {
                if(node == NULL || node->children[key[i]].tag == UNUSED) {
                    return std::make_pair<K, bucket_t*>(key, NULL);
                } else if(node->children[key[i]].tag == BUCKET) {
                    return std::make_pair<K, bucket_t*>(key.substr(i+1), node->children[key[i]].b);
                }
                node = node->children[key[i]].n;
                i++;
            }
        }
        V *find_in_bucket(bucket_t *b, K key) {
            typename std::vector<std::pair<K, V*> >::iterator it;
            it = std::lower_bound(b->contents.begin(), b->contents.end(), std::make_pair<K, V*>(key, NULL));
            if((*it).first == key) return (*it).second;
            return NULL;
        }
        bool delete_from_trie(child *c, K key) {
            if(key[0] != '\0') {
                delete_from_trie(c->children[key[0]], key.substr(1));
            } else {
                
            }
            return true;
        }

        bool delete_from_bucket(bucket_t *b, K key) {
            typename std::vector<std::pair<K, V*> >::iterator it;
            it = std::lower_bound(b->contents.begin(), b->contents.end(), std::make_pair<K, V*>(key, NULL));
            if((*it).first != key)
                return false;
            b->contents.erase(it);
            return true;
        }

        /*
         * Insertions
         * */
        void burst(child *c, char index) {
            //std::cout << "bursting bucket"<<c<<"["<<index<<"]"<<std::endl;
            //if(c->tag != BUCKET) exit(1);
            c->tag = NODE;

            node_t *newnode = create_node(index);
            for(int i = 0; i < c->b->contents.size(); i++) {
                insert_into_node(newnode, c->b->contents[i].first, c->b->contents[i].second);
            }
            bucket_t *temp = c->b;
            c->n = newnode;
            delete_bucket(temp);
        }
        void insert_into_node(node_t *node, K key, V *value) {
            int i = 0;
            child *p;
            while(key[i] != '\0') {
                if(node->children[key[i]].tag != UNUSED) {
                    p = &node->children[key[i]];
                    if(p->tag == NODE) {
                        node = p->n;
                    } else if(p->tag == BUCKET) {
                        bucket_t *b = (bucket_t*)p->b;
                        insert_into_bucket(b, key.substr(i+1), value);
                        // Bursting?
                        if(b->contents.size() > BUCKETCAPACITY)
                            burst(p, key[i]);
                        return;
                    }
                } else {
                    p = &node->children[key[i]];
                    p->tag = BUCKET;
                    p->b = create_bucket();
                    return insert_into_bucket(p->b, key.substr(i+1), value);
                }
                i++;
            }
            // \0 handling?
        }
        void insert_into_bucket(bucket_t *b, K key, V *value) {
            b->contents.insert(
                    std::lower_bound(b->contents.begin(),
                    b->contents.end(), std::make_pair<K, V*>(key, value)),
                std::make_pair<K, V*>(key, value));
        }
    public:
        explicit trie() {
            memset(root_node.children, 0, sizeof(root_node.children));
            root.tag = NODE;
            root.n = &root_node;
            nodes_created = 0;
            buckets_created = 0;
        }

        ~trie() {
            delete_node(&root);
            std::cout << buckets_created << " buckets were made\n";
            std::cout << nodes_created << " nodes were made\n";
        }

        V *search(K key) {
            //std::cout << "searching for: " << key << std::endl;
            std::pair<K, bucket_t *> nodedata = find_bucket(key);
            if(nodedata.second == NULL) return NULL;
            return find_in_bucket(nodedata.second, nodedata.first);
        }

        void insert(K key, V *value) {
            insert_into_node(&root_node, key, value);
        }

        void erase(K key) {
            std::pair<K, bucket_t *> nodedata = find_bucket(key);
            if(nodedata.second == NULL) return;
            if(delete_from_bucket(nodedata.second, nodedata.first)) {
                if(nodedata.second->contents.size() == 0)
                    delete_bucket(nodedata.second);
            }
        }
};
