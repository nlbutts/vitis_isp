#ifndef _SIMPLE_HDR_CONFIG_
#define _SIMPLE_HDR_CONFIG_

#include "hls_stream.h"
#include "common/xf_common.hpp"
#include "common/xf_utility.hpp"
#include "imgproc/xf_gaussian_filter.hpp"
#include "build/xf_config_params.h"

typedef unsigned short int uint16_t;

#define WIDTH 128
#define HEIGHT 128

#if GRAY
#define TYPE XF_8UC1
#define CH_TYPE XF_GRAY
#else
#define TYPE XF_8UC3
#define CH_TYPE XF_RGB
#endif

#if FILTER_SIZE_3
#define FILTER_WIDTH 3
#define FILTER 3
#elif FILTER_SIZE_5
#define FILTER_WIDTH 5
#define FILTER 5
#elif FILTER_SIZE_7
#define FILTER_WIDTH 7
#define FILTER 7
#endif

#if NO
#define NPC1 XF_NPPC1
#endif
#if RO
#define NPC1 XF_NPPC8
#endif

#define INPUT_PTR_WIDTH 128
#define OUTPUT_PTR_WIDTH 128

void simple_hdr(ap_uint<INPUT_PTR_WIDTH>* img_inp, ap_uint<OUTPUT_PTR_WIDTH>* img_out, int rows, int cols, int K1, int K2, int sigma1, int sigma2);
void div_test(ap_uint<16> pixel1, ap_uint<16> pixel2, ap_uint<8> shift, ap_uint<16> *result);
#endif // _SIMPLE_HDR_CONFIG_
