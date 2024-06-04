#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

//#include <yaml-cpp/yaml.h>
//#define RYML_SINGLE_HDR_DEFINE_NOW
//#include <ryml_all.hpp>

#if defined(_WIN32) | defined(__WIN32__) | defined(__WIN32) | defined(_WIN64) | defined(__WIN64)
#include <windows.h>
#endif

// C/C++ includes
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <string>
#include <vector>
#include <algorithm>
#include <type_traits>
#include <thread>
#include <complex>
#include <mutex>
#include <random>
#include <iostream>
#include <iomanip>

// custom includes
#include "test_gen.h"
#include "test_gen_lib.h"

// #define M_PI 3.14159265358979323846
// #define M_2PI 6.283185307179586476925286766559

//--------------------------------GLOBALS--------------------------------------
test_generator tg;

//-----------------------------------------------------------------------------
// template<typename T>
// void save_complex_data(std::string filename, std::vector<std::complex<T>> data)
// {
    // std::ofstream data_file;

    // data_file.open(filename, ios::out | ios::binary);

    // if (!data_file.is_open())
    // {
        // std::cout << "Could not save data. Closing... " << std::endl;
        // return;
    // }

    // data_file.write(reinterpret_cast<const char*>(data.data()), 2 * data.size() * sizeof(T));

    // data_file.close();
// }


//-----------------------------------------------------------------------------
void init_generator(float amplitude, unsigned int sample_rate, float half_bit_length, unsigned int filter_cutoff, unsigned int num_bits, int *ch, unsigned int num_channels)
{
    std::vector<int32_t> channels(ch, ch + num_channels);

    //test_generator tmp(amplitude, sample_rate, half_bit_length, filter_cutoff, num_bits, channels);
    ////tg(tmp);
    //tg = test_generator(amplitude, sample_rate, half_bit_length, filter_cutoff, num_bits, channels);

    tg.init_generator(amplitude, sample_rate, half_bit_length, filter_cutoff, num_bits, channels);
    
    std::cout << "Init sucessful!" << std::endl;

}   // end of init_generator

//-----------------------------------------------------------------------------
void generate_random_bursts(unsigned int num_bursts, unsigned int num_bits, short *iq_ptr, int*data_size)
{
    
    std::vector<std::complex<int16_t>> iq_data;
    
    tg.gen_rand_bursts(num_bursts, num_bits, iq_data);

    *data_size = (int32_t)(iq_data.size() * 2);

    std::copy((int16_t*)iq_data.data(), (int16_t*)iq_data.data() + *data_size, iq_ptr);
    
}   // end of generate_random_bursts

//-----------------------------------------------------------------------------
void generate_random_bursts_st(unsigned int num_bursts, unsigned int num_bits, iq_data_struct *iq_ptr, int* data_size)
{

    std::vector<std::complex<int16_t>> iq_data;

    tg.gen_rand_bursts(num_bursts, num_bits, iq_ptr, *data_size);

    //*data_size = (int32_t)(iq_data.size() * 2);

    //std::copy((int16_t*)iq_data.data(), (int16_t*)iq_data.data() + *data_size, iq_ptr);

}   // end of generate_random_bursts

//-----------------------------------------------------------------------------
int calcuate_num_samples(unsigned int num_bursts, unsigned int num_bits)
{
    int data_size = 0;

    // calculate bits per symbol
    uint32_t samples_per_bit = floor(tg.sample_rate * tg.half_symbol_length);
    uint32_t samples_per_symbol = samples_per_bit << 1;

    // check for odd numberand append a 0 at the end if it is odd
    if (num_bits % 2 == 1)
        ++num_bits;

    uint32_t num_bit_pairs = num_bits >> 1;

    data_size = (num_bit_pairs * samples_per_symbol) + samples_per_bit;

    return data_size;

}   // end of calcuate_num_samples

/*
//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
    std::string sdate, stime;

    uint64_t idx=0, jdx=0;

    typedef chrono::nanoseconds ns;
    auto start_time = chrono::high_resolution_clock::now();
    auto stop_time = chrono::high_resolution_clock::now();
    auto elapsed_time = chrono::duration_cast<ns>(stop_time - start_time);

    int bp = 0;

    get_platform(platform);
    std::cout << "Platform: " << platform << std::endl << std::endl;


    try
    {
#if defined(_WIN32) | defined(__WIN32__) | defined(__WIN32) | defined(_WIN64) | defined(__WIN64)

#else
        std::cout << "argv[0]: " << std::string(argv[0]) << std::endl;
        std::string exe_path = get_ubuntu_path();
        std::cout << "Path: " << exe_path << std::endl;
#endif

        //----------------------------------------------------------------------------------------
        // variables
        uint32_t num_threads; 
        uint32_t num_loops;
        uint32_t num_blocks;
        

        
        num_loops = 100;


        std::default_random_engine generator(10);
        std::uniform_int_distribution<int32_t> distribution(0, 1);

        std::vector<uint8_t> data;
        float amplitude = 2000;
        uint32_t sample_rate = 52000000;
        float half_bit_length = 0.00000025;
        uint32_t fc = 1200000;

        uint32_t num_bits = 208;
        uint32_t num_bursts = 16;
        //for (idx = 0; idx < num_bits; ++idx)
        //    data.push_back(distribution(generator));

        //data.clear();
        //for (idx = 0; idx < num_bits; ++idx)
        //    data.push_back(distribution(generator));

        std::vector<int32_t> channels = { -8000000, -7000000, -6000000, -5000000, -4000000, -3000000, -2000000, -1000000, 1000000, 2000000, 3000000, 4000000, 5000000, 6000000, 7000000, 8000000 };

        std::vector<std::complex<int16_t>> iq_data;
        burst_generator bg(amplitude, sample_rate, half_bit_length, fc, num_bits, channels);

        //bg.generate_channel_rot(num_bits);

        double run_time_sum = 0.0;

        for(idx=0; idx<1; ++idx)
        {
            start_time = chrono::high_resolution_clock::now();

            bg.generate_random_bursts(num_bursts*16, num_bits, iq_data);

            stop_time = chrono::high_resolution_clock::now();

            const auto int_ms = std::chrono::duration_cast<std::chrono::microseconds>(stop_time - start_time);

            std::cout << "elapsed_time: " << int_ms.count()/1e6 << std::endl;

            run_time_sum += int_ms.count() / 1.0e6;
        }

        std::cout << "average elapsed_time: " << run_time_sum/100.0 << std::endl;
        save_complex_data("D:/Projects/data/RF/test_oqpsk_burst.sc16", iq_data);

        std::cin.ignore();


        bp = 10;
        





	return 0;

}	// end of main

*/
