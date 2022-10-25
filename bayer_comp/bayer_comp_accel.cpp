


#ifdef __SYNTHESIS__
#include "bayer_comp_accel.hpp"

// void bayer_comp_accel_imp(hls::stream<pixel> &src,
//                       hls::stream<pixel> &dst0,
//                       hls::stream<pixel> &dst1,
//                       hls::stream<pixel> &dst2,
//                       hls::stream<pixel> &dst3,
//                       int width, int height)
// {
// #pragma HLS INTERFACE axis port=src
// #pragma HLS INTERFACE axis port=dst0
// #pragma HLS INTERFACE axis port=dst1
// #pragma HLS INTERFACE axis port=dst2
// #pragma HLS INTERFACE axis port=dst3
//     bayer_comp_accel_imp(src, dst0, dst1, dst2, dst3, width, height);
// }

int Rice_Compress_accel( int16_t *in, void *out, unsigned int insize, int k )
{
#pragma HLS INTERFACE axis port=in
#pragma HLS INTERFACE axis port=out
    return Rice_Compress(in, out, insize, k);
}


#endif
