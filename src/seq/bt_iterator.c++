#ifndef __BT_ITERATOR
#define __BT_ITERATOR

template<
    typename B
    >
class bt_iterator {
    typedef typename B::value_type value_type;
    typedef typename B::key_type key_type;
    typedef bt_iterator<B> self_type;
    B * currentBucket;
    typename B::iterator it;
    unsigned int bucketnumber;

    public:
        explicit bt_iterator(B *b, bool toEnd = false) {
            currentBucket = b;
            it = b->begin();
            if(toEnd) it = b->end();
        }
        bool operator==(self_type other) {
            return (this->currentBucket == other.currentBucket && this->it == other.it);
        }
        bool operator!=(self_type other) {
            return (this->currentBucket != other.currentBucket || this->it != other.it);
        }
        value_type *operator*() {
            return (*it).second;
        }
        self_type &operator++() {
            it++;
            if(it == currentBucket->end() && currentBucket->next) {
                currentBucket = currentBucket->next;
                it = currentBucket->begin();
                bucketnumber++;
            }
            return *this;
        }
};

#endif
