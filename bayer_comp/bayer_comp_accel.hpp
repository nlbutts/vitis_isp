#ifndef _BAYER_COMP_ACCEL_HPP_
#define _BAYER_COMP_ACCEL_HPP_

#include <stdlib.h>

int Rice_Compress( int16_t *in, void *out, unsigned int insize, int k );
int Rice_Compress_accel( int16_t *in, void *out, unsigned int insize, int k );

#endif // _BAYER_COMP_ACCEL_HPP_