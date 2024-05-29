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
    LIB void init_vs_gen_from_file(const char* fn);

    //-----------------------------------------------------------------------------
    LIB void init_vs_generator(unsigned int sig_tbl_num,
        double* sigma_table_t,
        unsigned int blur_tbl_num,
        unsigned char* dm_values_t,
    );

    //-----------------------------------------------------------------------------
    LIB void set_vs_seed(int seed);

    //-----------------------------------------------------------------------------
    LIB void get_vs_minmax(unsigned short* min_dm_value, unsigned short* max_dm_value);

    //-----------------------------------------------------------------------------
    LIB void set_vs_shape_scale(double s);

#ifdef __cplusplus
}
#endif

#endif  // _TEST_GEN_LIB_H_
