#define _CRT_SECURE_NO_WARNINGS

#if defined(_WIN32) | defined(__WIN32__) | defined(__WIN32) | defined(_WIN64) | defined(__WIN64)

#include <windows.h>

#else
    
//#include <dlfcn.h>
typedef void* HINSTANCE;

#endif

// C/C++ includes
//#include <stdint.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
// #include <fstream>
// #include <iomanip>
#include <chrono>
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
    
    typedef std::chrono::nanoseconds ns;
    auto start_time = std::chrono::high_resolution_clock::now();
    auto stop_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<ns>(stop_time - start_time);

    //----------------------------------------------------------------------------------------
    // variables    
    unsigned int idx = 0;
    float amplitude = 2000.0f;
    unsigned int sample_rate = 52000000;
    unsigned int fc = 1200000;
    float half_bit_length = 0.00000025f;

    unsigned int num_bits = 208;
    unsigned int num_bursts = 1;
    int data_size = 0;
    
    int channels[1] = {0};
    
    iq_data_struct iq_data[6000] = {0};

    // run the lib initialization routine
    init_generator(amplitude, sample_rate, half_bit_length, fc, num_bits, channels, 1);

    // calculate the data size based on the num_bursts and num_bits
    data_size = calcuate_num_samples(num_bursts, num_bits);
    std::cout << "number of samples generated: " << data_size << std::endl;

    // generate the random bursts
    start_time = std::chrono::high_resolution_clock::now();
    generate_random_bursts_st(num_bursts, num_bits, iq_data, &data_size);
    stop_time = std::chrono::high_resolution_clock::now();
    elapsed_time = std::chrono::duration_cast<ns>(stop_time - start_time);

    for(idx=0; idx<data_size; ++idx)
    {
        //printf("Sample: %d \t %di\r\n", iq_data[idx], iq_data[idx+1]);
        std::cout << "Sample: " << iq_data[idx].r << "\t" << iq_data[idx].i << std::endl;
    }

    std::cout << std::endl << "time (us): " << elapsed_time.count() / 1000.0 << std::endl;

	return 0;

}	// end of main

