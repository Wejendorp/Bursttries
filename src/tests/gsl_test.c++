/*
By Jacob Wejendorp 
*/
#include <fstream>
#include <iostream>
#include <gsl/gsl_rng.h>

//
// Testing the random number generator
//
int main(int argc, char *argv[]) {
    const gsl_rng_type * T;
    gsl_rng * r;
    int n = 100000;

    gsl_rng_env_setup();

    T = gsl_rng_default;
    r = gsl_rng_alloc(T);

    for(int i = 0; i < n; i++) {
        int u = gsl_rng_uniform_int(r, 128);
        std::cout << u << std::endl;
    }
    gsl_rng_free(r);
    return 0;
}

