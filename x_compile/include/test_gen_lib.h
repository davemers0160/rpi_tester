#ifndef _TEST_GEN_LIB_H_
#define _TEST_GEN_LIB_H_

#if defined(_WIN32) | defined(__WIN32__) | defined(__WIN32) | defined(_WIN64) | defined(__WIN64)

    #ifdef DLL_EXPORTS
        #define LIB __declspec(dllexport)
    #else
        #define LIB __declspec(dllimport)
    #endif

#else
    #define LIB

#endif


typedef struct {
    short r;
    short i;
} iq_data_struct;

//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

    //-----------------------------------------------------------------------------
    LIB void init_generator(float amplitude, unsigned int sample_rate, float half_bit_length, unsigned int filter_cutoff, unsigned int num_bits, int *ch, unsigned int num_channels);

    //-----------------------------------------------------------------------------
    LIB void generate_random_bursts(unsigned int num_bursts, unsigned int num_bits, short* iq_ptr, int* data_size);
    LIB void generate_random_bursts_st(unsigned int num_bursts, unsigned int num_bits, iq_data_struct* iq_ptr, int* data_size);

    //-----------------------------------------------------------------------------
    LIB int calcuate_num_samples(unsigned int num_bursts, unsigned int num_bits);

#ifdef __cplusplus
}
#endif

#endif  // _TEST_GEN_LIB_H_
