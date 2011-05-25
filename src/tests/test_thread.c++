#include <pthread.h>
#include <string>
#include <iostream>
template<
    typename inputType,
    typename outputType
>
class test_thread {
    private:
        pthread_t id;
        inputType *in;
        outputType *out;
        static void* write_dispatch(void *arg) {
            reinterpret_cast<test_thread*>(arg)->write_routine();
            return NULL;
        }
        void *write_routine() {
            std::string *val;
            while((val = in->getNext()) != NULL) {
                out->insert(std::make_pair<std::string, std::string*>(*val, val));
            }
            pthread_exit(NULL);
        }
        static void* read_dispatch(void *arg) {
            reinterpret_cast<test_thread*>(arg)->write_routine();
            return NULL;
        }
        void *read_routine() {
            std::string *val;
            while((val = in->getNext()) != NULL) {
                if(val != out->find(*val))
                    std::cout << std::endl;
            }
            pthread_exit(NULL);
        }

    public:
        explicit test_thread(inputType *input, outputType *output) {
            in = input;
            out = output;
        }
        ~test_thread() {
            //join();
        }
        void write() {
            pthread_create(&id, NULL, test_thread::write_dispatch,
                    reinterpret_cast<void*>(this));
        }
        void read() {
            pthread_create(&id, NULL, test_thread::read_dispatch,
                    reinterpret_cast<void*>(this));
        }
        void join() {
            pthread_join(id, NULL);
        }
};
