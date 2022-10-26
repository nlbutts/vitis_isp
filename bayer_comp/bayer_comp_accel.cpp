#include <stdlib.h>
#include "hls_stream.h"
#include "ap_axi_sdata.h"

#define RICE_HISTORY    16
#define RICE_WORD       16
#define BITS            16
#define RICE_THRESHOLD  8

typedef ap_axiu<BITS, 1, 1, 1> pixel;

typedef struct {
    unsigned char *BytePtr;
    unsigned int  index;
    unsigned int  NumBytes;
    unsigned int  tempbits;
    unsigned int  tempcount;
} rice_bitstream_t;

/*************************************************************************
* _Rice_NumBits() - Determine number of information bits in a word.
*************************************************************************/

static int _Rice_NumBits( unsigned int x )
{
    int n;
    for( n = 32; !(x & 0x80000000) && (n > 0); -- n ) x <<= 1;
    return n;
}


/*************************************************************************
* _Rice_InitBitstream() - Initialize a bitstream.
*************************************************************************/

static void _Rice_InitBitstream( rice_bitstream_t *stream,
    void *buf, unsigned int bytes )
{
    stream->BytePtr   = (unsigned char *) buf;
    stream->index     = 0;
    stream->NumBytes  = bytes;
    stream->tempbits  = 0;
    stream->tempcount = 0;

}


/*************************************************************************
* _Rice_WriteBit() - Write a bit to the output stream.
*************************************************************************/

static void _Rice_WriteBit( rice_bitstream_t *stream, int x )
{
    if( stream->index < stream->NumBytes )
    {
        stream->tempbits |= x;
        stream->tempcount++;
        if (stream->tempcount >= 8)
        {
            stream->BytePtr[stream->index] = stream->tempbits & 0xFF;
            stream->tempbits = 0;
            stream->tempcount = 0;
            stream->index++;
        }
        else
        {
            stream->tempbits <<= 1;
        }
    }
}


/*************************************************************************
* _Rice_EncodeWord() - Encode and write a word to the output stream.
*************************************************************************/

static void _Rice_EncodeWord( unsigned int x, int k,
    rice_bitstream_t *stream )
{
    unsigned int q, i;
    int          j, o;

    /* Determine overflow */
    q = x >> k;

    /* Too large rice code? */
    if( q > RICE_THRESHOLD )
    {
        /* Write Rice code (except for the final zero) */
        for( j = 0; j < RICE_THRESHOLD; ++ j )
        {
            _Rice_WriteBit( stream, 1 );
        }

        /* Encode the overflow with alternate coding */
        q -= RICE_THRESHOLD;

        /* Write number of bits needed to represent the overflow */
        o = _Rice_NumBits( q );
        for( j = 0; j < o; ++ j )
        {
            _Rice_WriteBit( stream, 1 );
        }
        _Rice_WriteBit( stream, 0 );

        /* Write the o-1 least significant bits of q "as is" */
        for( j = o-2; j >= 0; -- j )
        {
            _Rice_WriteBit( stream, (q >> j) & 1 );
        }
    }
    else
    {
        /* Write Rice code */
        for( i = 0; i < q; ++ i )
        {
            _Rice_WriteBit( stream, 1 );
        }
        _Rice_WriteBit( stream, 0 );
    }

    /* Encode the rest of the k bits */
    for( j = k-1; j >= 0; -- j )
    {
        _Rice_WriteBit( stream, (x >> j) & 1 );
    }
}

int Rice_Compress( int16_t *in, void *out, unsigned int insize, int k )
{
    rice_bitstream_t stream;
    unsigned int     i, x, n, incount;
    unsigned int     hist[ RICE_HISTORY ];
    int              j, sx;

    incount = insize / (RICE_WORD>>3);

    /* Initialize output bitsream */
    _Rice_InitBitstream( &stream, out, insize+1 );

    /* Encode input stream */
    for( i = 0; (i < incount) && (stream.index <= insize); ++ i )
    {
        /* Revise optimum k? */
        if( i >= RICE_HISTORY )
        {
            k = 0;
            for( j = 0; j < RICE_HISTORY; ++ j )
            {
                k += hist[ j ];
            }
            k = (k + (RICE_HISTORY>>1)) / RICE_HISTORY;
        }

        /* Read word from input buffer */
        sx = in[i];
        x = sx < 0 ? -1-(sx<<1) : sx<<1;

        /* Encode word to output buffer */
        _Rice_EncodeWord( x, k, &stream );

        /* Update history */
        hist[ i % RICE_HISTORY ] = _Rice_NumBits( x );
    }

    /* Was there a buffer overflow? */
    if( i < incount )
    {
        //printf("OVERFLOW\n");
    }
    else
    {
        // Flush the last few bits
        stream.BytePtr[stream.index] = (stream.tempbits << (7 - stream.tempcount)) & 0xFF;
    }

    return stream.index;
}

void write_dst_port(ap_uint<2> index,
                    pixel out_p,
                    hls::stream<pixel> &dst0,
                    hls::stream<pixel> &dst1,
                    hls::stream<pixel> &dst2,
                    hls::stream<pixel> &dst3)
{
    switch (index)
    {
        case 0:
            dst0.write(out_p);
            break;
        case 1:
            dst1.write(out_p);
            break;
        case 2:
            dst2.write(out_p);
            break;
        case 3:
            dst3.write(out_p);
            break;
    }
}


//template <int TYPE>
void bayer_comp_accel(hls::stream<pixel> &src,
                      hls::stream<pixel> &dst0,
                      hls::stream<pixel> &dst1,
                      hls::stream<pixel> &dst2,
                      hls::stream<pixel> &dst3,
                      int width, int height)
{
#pragma HLS INTERFACE axis port=src
#pragma HLS INTERFACE axis port=dst0
#pragma HLS INTERFACE axis port=dst1
#pragma HLS INTERFACE axis port=dst2
#pragma HLS INTERFACE axis port=dst3
    bool eos = false;
    ap_uint<12> row = 0;
    ap_uint<12> col = 0;
    bool first_pixel[4] = {false};
    ap_uint<BITS> previous[4] = {0};
    bool last = false;

    do
    {
        pixel in_p = src.read();

        if (in_p.user)
        {
            row = 0;
            col = 0;
            first_pixel[0] = false;
            first_pixel[1] = false;
            first_pixel[2] = false;
            first_pixel[3] = false;
        }

        ap_uint<2> index = (col & 1) + ((row & 1) << 1);

        if ((row == (height - 2)) && (col >= (width - 2)))
        {
            last = true;
            //printf("row: %d col: %d\n", row, col);
        }
        else if ((row == (height - 1)) && (col >= (width - 2)))
        {
            last = true;
            //printf("row: %d col: %d\n", row, col);
        }
        else
        {
            last = false;
        }

        col++;
        if (col >= width)
        {
            col = 0;
            row++;
        }
        if (row >= height)
        {
            eos = true;
        }

        // Even line
        pixel out_p;
        out_p.user = 0;
        out_p.last = 0;
        // Even pixel
        if (!first_pixel[index])
        {
            first_pixel[index] = true;
            out_p.user = 1;
            out_p.data = in_p.data;
            //dst[index].write(out_p);
            write_dst_port(index, out_p, dst0, dst1, dst2, dst3);
        }
        else
        {
            ap_uint<BITS> diff = in_p.data - previous[index];
            out_p.data = diff;
            out_p.last = last;
            //dst[index].write(out_p);
            write_dst_port(index, out_p, dst0, dst1, dst2, dst3);
        }

        previous[index] = in_p.data;
    } while (eos == false);
}

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
