#define _CRT_SECURE_NO_WARNINGS

#if defined(_WIN32) | defined(__WIN32__) | defined(__WIN32) | defined(_WIN64) | defined(__WIN64)

#include <windows.h>

#else
    
#include <dlfcn.h>
typedef void* HINSTANCE;

#endif

// C/C++ includes
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
// #include <iostream>
// #include <fstream>
// #include <iomanip>
// #include <chrono>
// #include <string>
// #include <vector>
// #include <array>
// #include <algorithm>
// #include <type_traits>
// #include <list>
// #include <thread>
// #include <complex>
// #include <mutex>
// #include <random>

#include "test_gen_lib.h"



// -------------------------------GLOBALS--------------------------------------


// ----------------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    
    //----------------------------------------------------------------------------------------
    // variables    
    unsigned int idx = 0;
    float amplitude = 2000.0f;
    unsigned int sample_rate = 52000000;
    unsigned int fc = 1200000;
    float half_bit_length = 0.00000025f;

    unsigned int num_bits = 208;
    unsigned int num_bursts = 1;
    unsigned int data_size = 0;
    
    int channels[1] = {0};
    
    short iq_data[6000] = {0};

    init_generator(amplitude, sample_rate, half_bit_length, fc, num_bits, channels, 1);

    generate_random_bursts(num_bursts, num_bits, iq_data, &data_size);
    
    printf("number of samples generated: %d", data_size);
    
    for(idx=0; idx<data_size; idx=idx+2)
    {
        printf("Sample: %d \t %di\r\n", iq_data[idx], iq_data[idx+1]);
    }

	return 0;

}	// end of main

