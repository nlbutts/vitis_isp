#ifndef _BAYER_COMP_ACCEL_HPP_
#define _BAYER_COMP_ACCEL_HPP_


#include "hls_stream.h"
#include "ap_axi_sdata.h"
#include "common/xf_common.hpp"
#include "common/xf_utility.hpp"
#include "imgproc/xf_gaussian_filter.hpp"

#define BITS 16

typedef ap_axiu<BITS, 1, 1, 1> pixel;

//template <int TYPE>
void bayer_comp_accel(hls::stream<pixel> &src,
                      hls::stream<pixel> dst[4],
                      int width, int height)
{
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
            printf("row: %d col: %d\n", row, col);
        }
        else if ((row == (height - 1)) && (col >= (width - 2)))
        {
            last = true;
            printf("row: %d col: %d\n", row, col);
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
            dst[index].write(out_p);
        }
        else
        {
            ap_uint<BITS> diff = in_p.data - previous[index];
            out_p.data = diff;
            out_p.last = last;
            dst[index].write(out_p);
        }

        previous[index] = in_p.data;
    } while (eos == false);
}

#endif // _BAYER_COMP_ACCEL_HPP_