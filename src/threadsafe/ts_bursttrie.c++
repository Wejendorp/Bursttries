
template<
    typename N,
    typename B
>
class ts_bursttrie {
    private:
        N *root;
        typename K N::K;
        typename V N::V;

    public:
        explicit ts_bursttrie() {
            root = new N;
        }
        ~ts_bursttrie() {
            delete(root);
        }
        void remove(K key) {
            return root->remove(key);
        }
        V *find(K key) {
            return root->find(key);
        }
        void insert(K key, V *value) {
            return root->insert(key, value);
        }
}
