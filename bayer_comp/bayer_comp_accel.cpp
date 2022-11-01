#include <stdlib.h>
#include "ap_axi_sdata.h"
#include "bayer_comp_accel.hpp"

#define RICE_HISTORY    16
#define RICE_WORD       16
#define BITS            16
#define RICE_THRESHOLD  8

typedef ap_axiu<BITS, 1, 1, 1> pixel;
typedef ap_axiu<8, 1, 1, 1> compdata;

typedef struct {
    hls::stream<uint8_t>*   outdata;
    unsigned int            index;
    ap_uint<9>              tempbits;
} rice_bitstream_t;

/*************************************************************************
* _Rice_NumBits() - Determine number of information bits in a word.
*************************************************************************/

static int _Rice_NumBits( uint16_t x )
{
    //int n = 0;
    uint8_t n = 0;
    if (x)
    {
        n = 32 - __builtin_clz(x);
    }
    //for( n = 32; !(x & 0x80000000) && (n > 0); -- n ) x <<= 1;
    //return n;
    //printf("_Rice_NumBits: %d %d\n", n, temp);
    return n;
}


/*************************************************************************
* _Rice_InitBitstream() - Initialize a bitstream.
*************************************************************************/

static void _Rice_InitBitstream( rice_bitstream_t *stream,
    hls::stream<uint8_t> &outdata )
{
    stream->outdata     = &outdata;
    stream->tempbits    = 1;
    stream->index       = 0;
}


/*************************************************************************
* _Rice_WriteBit() - Write a bit to the output stream.
*************************************************************************/

static void _Rice_WriteBit( rice_bitstream_t *stream, bool x )
{
    // if( stream->index < stream->NumBytes )
    // {
    //     stream->tempbits |= x;
    //     stream->tempcount++;
    //     if (stream->tempcount >= 8)
    //     {
    //         stream->BytePtr[stream->index] = stream->tempbits & 0xFF;
    //         stream->tempbits = 0;
    //         stream->tempcount = 0;
    //         stream->index++;
    //     }
    //     else
    //     {
    //         stream->tempbits <<= 1;
    //     }
    // }
    stream->tempbits <<= 1;
    stream->tempbits[0] = x;
    if (stream->tempbits & 0x100)
    {
        stream->outdata->write(stream->tempbits & 0xFF);
        stream->tempbits = 1;
        stream->index++;
    }
}


/*************************************************************************
* _Rice_EncodeWord() - Encode and write a word to the output stream.
*************************************************************************/

static void _Rice_EncodeWord( uint16_t x,
                              ap_uint<5> k,
                              rice_bitstream_t *stream )
{
    //unsigned int q, i;
    uint8_t q, i;
    //int8_t          j, o;
    int8_t          j;
    ap_uint<5>      o;
    //int          j, o;

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


//int Rice_Compress( int16_t *in, uint8_t *out, unsigned int insize, int k )
int Rice_Compress(hls::stream<int16_t> &indata,
                  hls::stream<uint8_t> &outdata,
                  unsigned int insize,
                  uint16_t k )
{
    rice_bitstream_t    stream;
    unsigned int        i, incount;
    ap_uint<5>          hist[ RICE_HISTORY ];
    ap_uint<5>          j;
    int16_t             sx;
    uint16_t            x;

    _Rice_InitBitstream(&stream, outdata);

    //incount = insize / (RICE_WORD>>3);
    // how many 8-bit values from 16-bit inputs
    incount = insize >> 1;

    /* Encode input stream */
    //for( i = 0; (i < incount) && (stream.index <= insize); ++ i )
    for( i = 0; i < incount; ++ i )
    {
        /* Revise optimum k? */
        if( i >= RICE_HISTORY )
        {
            k = 0;
            for( j = 0; j < RICE_HISTORY; ++ j )
            {
                k += hist[ j ];
            }
            //k = (k + (RICE_HISTORY>>1)) / RICE_HISTORY;
            // RICE_HISTORY is 16 which is a shift by 4
            k = (k + (RICE_HISTORY>>1)) >> 4;
        }

        /* Read word from input buffer */
        sx = indata.read();
        x = sx < 0 ? -1-(sx<<1) : sx<<1;

        /* Encode word to output buffer */
        _Rice_EncodeWord( x, k, &stream );

        /* Update history */
        hist[ i % RICE_HISTORY ] = _Rice_NumBits( x );
    }

    // /* Was there a buffer overflow? */
    // if( i < incount )
    // {
    //     //printf("OVERFLOW\n");
    // }
    // else
    // {
    // }

    // Flush the last few bits
    while (!(stream.tempbits & 0x100))
    {
        stream.tempbits <<= 1;
    }
    //uint8_t finalbyte = (stream.tempbits << (7 - stream.tempcount)) & 0xFF;
    outdata.write(stream.tempbits & 0xFF);
    //stream.BytePtr[stream.index] = (stream.tempbits << (7 - stream.tempcount)) & 0xFF;

    return stream.index + 1;
}

#if 0
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
                      hls::stream<compdata> &dst,
                      int width, int height)
{
#pragma HLS INTERFACE axis port=src
#pragma HLS INTERFACE axis port=dst
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
#endif

int Rice_Compress_accel( hls::stream<int16_t> &indata,
                         hls::stream<uint8_t> &outdata,
                         unsigned int insize,
                         int k )
{
#pragma HLS INTERFACE mode=s_axilite port=return
#pragma HLS INTERFACE mode=s_axilite port=insize
#pragma HLS INTERFACE mode=s_axilite port=k
//#pragma HLS DATAFLOW
#pragma HLS INTERFACE axis port=indata
#pragma HLS INTERFACE axis port=outdata
    return Rice_Compress(indata, outdata, insize, k);
}
