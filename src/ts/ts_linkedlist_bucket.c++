#ifndef __TS_LL_BUCKET
#define __TS_LL_BUCKET

template<
    typename N
>
class ts_linkedlist_bucket {
    public:
        typedef typename N::value_type value_type;
        typedef typename N::key_type key_type;
        typedef N node;
        volatile unsigned int capacity, size;
        pthread_rwlock_t rwlock;

        class linkedlist_node {
            public:
            key_type k;
            value_type v;
            linkedlist_node * next;

            linkedlist_node(key_type key, value_type value, linkedlist_node *n) {
                k = key;
                v = value;
                next = n;
            }
            ~linkedlist_node() {
                if(next) delete(next);
            }
        };
        linkedlist_node * head;

        explicit ts_linkedlist_bucket(int cap) {
            capacity = cap;
            size = 0;
            head = NULL;
            pthread_rwlock_init(&rwlock, NULL);
            AO_fetch_and_add1(atomic BUCKET_COUNT);
        }
        ~ts_linkedlist_bucket() {
            if(head) delete(head);
            AO_fetch_and_add1(atomic BUCKETS_DESTROYED);
            pthread_rwlock_destroy(&rwlock);
        }
        void insert(const key_type &k, const value_type &v, pthread_rwlock_t *oldLock = NULL) {
            pthread_rwlock_wrlock(&rwlock);
            if(oldLock) pthread_rwlock_unlock(oldLock);

            size++;
            head = new linkedlist_node(k, v, head);

            pthread_rwlock_unlock(&rwlock);
        }
        bool remove(const key_type &key, pthread_rwlock_t *oldLock = NULL) {
            bool ret = false;
            pthread_rwlock_wrlock(&rwlock);
            if(oldLock) pthread_rwlock_unlock(oldLock);

            linkedlist_node *cur = head;
            // Remove first node
            if(cur) {
                if(head->k == key) {
                    head = cur->next;
                    cur->next = NULL;
                    delete(cur);
                    ret = true;
                    size--;
                } else // remove successor of "cur"
                    while(cur->next != NULL) {
                        if(cur->next->k == key) {
                            linkedlist_node *temp = cur->next;
                            cur->next = cur->next->next;
                            temp->next = NULL;
                            delete(temp);
                            size--;
                            ret = true;
                            break;
                        } else {
                            cur = cur->next;
                        }
                    }
            }
            pthread_rwlock_unlock(&rwlock);
            return false;
        }
        value_type find(const key_type &key, pthread_rwlock_t *oldLock = NULL) {
            value_type ret = NULL;
            pthread_rwlock_rdlock(&rwlock);
            if(oldLock) pthread_rwlock_unlock(oldLock);

            linkedlist_node *cur = head;
            while(cur != NULL)
                if(cur->k == key) {
                    ret = cur->v;
                    break;
                } else
                    cur = cur->next;

            pthread_rwlock_unlock(&rwlock);
            return ret;
        }
        node *burst() {
            pthread_rwlock_wrlock(&rwlock);
            node *newnode = NULL;
            if(size > capacity) {
                size = 0;
                newnode = new node();

                linkedlist_node *cur = head;
                while(cur != NULL) {
                    newnode->insert(cur->k, cur->v);
                    cur = cur->next;
                }
            }
            pthread_rwlock_unlock(&rwlock);
            return newnode;
        }
};
#endif
