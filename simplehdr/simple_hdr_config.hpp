#ifndef _SIMPLE_HDR_CONFIG_
#define _SIMPLE_HDR_CONFIG_

#include "hls_stream.h"
#include "ap_int.h"
#include "common/xf_common.hpp"
#include "common/xf_utility.hpp"
//#include "imgproc/xf_demosaicing.hpp"
#include "xf_config_params.h"

#define INPUT_PTR_WIDTH 128
#define OUTPUT_PTR_WIDTH 128

// Resolve input and output pixel type:
#if T_8U
#define IN_TYPE XF_8UC1
#define OUT_TYPE XF_8UC3
#endif
#if T_16U
#define IN_TYPE XF_16UC1
#define OUT_TYPE XF_16UC3
#endif

// Resolve optimization type:
#define NPC1 NPPC

#if (T_16U || T_10U || T_12U)
#define CV_INTYPE CV_16UC1
#define CV_OUTTYPE CV_16UC3
#else
#define CV_INTYPE CV_8UC1
#define CV_OUTTYPE CV_8UC3
#endif

#define ERROR_THRESHOLD 1

void simple_hdr(ap_uint<INPUT_PTR_WIDTH>* img_in, ap_uint<OUTPUT_PTR_WIDTH>* img_out, int height, int width, int K1, int K2);
#endif // _SIMPLE_HDR_CONFIG_
