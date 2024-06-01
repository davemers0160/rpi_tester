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

//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

    //-----------------------------------------------------------------------------
    LIB void init_generator(float amplitude, uint32_t sample_rate, float half_bit_length, uint32_t filter_cutoff, uint32_t num_bits, int32_t *ch, uint32_t num_channels);

    //-----------------------------------------------------------------------------
    LIB void generate_random_bursts(uint32_t num_bursts, uint32_t num_bits, int16_t **iq_ptr, uint32_t *data_size);

#ifdef __cplusplus
}
#endif

#endif  // _TEST_GEN_LIB_H_
