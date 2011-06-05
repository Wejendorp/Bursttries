/*
By Jacob Wejendorp 
*/
#include <fstream>
#include <iostream>
#include <gsl/gsl_rng.h>
#include <string>
// Generate a random string with a given alphabet,
// and place it in the given string
//
void gen_random(std::string *s, gsl_rng *r) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    int len = gsl_rng_uniform_int(r, 7)+5;
    for(int i = 0; i < len; i++) {
        int u = gsl_rng_uniform_int(r, sizeof(alphanum));
        s->push_back(alphanum[u]);
    }
}

//
// Testing the random number generator
//
int main(int argc, char *argv[]) {
    // Generate random numbers 
    const gsl_rng_type * T;
    gsl_rng * r;
    gsl_rng_env_setup();

    T = gsl_rng_default;
    r = gsl_rng_alloc(T);

    for(int i = 0; i < TESTSIZE; i++) {
        std::string st = "";
        gen_random(&st, r);
        std::cout << st << std::endl;
    }
    gsl_rng_free(r);

    return 0;
}

