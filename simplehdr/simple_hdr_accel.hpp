#ifndef _SIMPLE_HDR_ACCEL_HPP_
#define _SIMPLE_HDR_ACCEL_HPP_

#ifndef __cplusplus
#error C++ is needed to include this header
#endif

#include "hls_stream.h"
#include "ap_axi_sdata.h"
#include "common/xf_common.hpp"
#include "common/xf_utility.hpp"
//#include "build/xf_config_params.h"

//Hp = GaussianBlur(img) + mean(img) / 2
//Ibip = (Icfa(max) + H(p)) * (Icfa(p) / (Icfa(p) + H(p)))

//Ap = GaussianBlur(Ibip) + mean(Ibip) / 2
//Iga = (Ibip(max) + A(p)) * (Ibip(p) / (Ibip(p) + A(p)))

#define BITS 16

typedef ap_axiu<BITS, 1, 1, 1> pixel;

template <int SRC_T>
ap_uint<SRC_T> calculate_box(ap_uint<SRC_T> * window)
{
    ap_uint<18> sum = 0;
    for (ap_uint<6> i = 0; i < 25; i++)
    {
        sum += window[i];
    }
    sum /= 25;
    return sum;
}

template <int SRC_T>
void box_filter(ap_uint<SRC_T> *line0,
                ap_uint<SRC_T> *line1,
                ap_uint<SRC_T> *line2,
                ap_uint<SRC_T> *line3,
                ap_uint<SRC_T> *line4,
                ap_uint<12> width,
                hls::stream<pixel>& dst,
                bool SOF)
{
    pixel dstp;
    ap_uint<SRC_T> window[25] = {0};
    for (ap_uint<12> x = 0; x < width; x++)
    {
        for (ap_int<3> win = 0; win < 5; win++)
        {
            ap_int<12> check1 = win - 2;
            ap_int<12> check2 = (win - 2) + x;
            if ((check1 < x) && (check2 >= width))
            {
                ap_int<12> offset = (win + 2) * 5;
                window[offset + 1] = 0;
                window[offset + 2] = 0;
                window[offset + 3] = 0;
                window[offset + 4] = 0;
                window[offset + 5] = 0;
            }
            else
            {
                ap_int<12> offset = (win + 2) * 5;
                window[offset + 1] = line0[x];
                window[offset + 2] = line1[x];
                window[offset + 3] = line2[x];
                window[offset + 4] = line3[x];
                window[offset + 5] = line4[x];
            }
        }
        dstp.data = calculate_box<SRC_T>(window);
        dstp.user = SOF;
        if (x == (width - 1))
        {
            dstp.last = true;
        }
        dst.write(dstp);
    }
}

template <int SRC_T,
          int ROWS,
          int COLS>
void simple_stream(hls::stream<pixel>& src, hls::stream<pixel>& dst, ap_uint<12> width, ap_uint<12> height)
{
    bool eos = false;
    ap_uint<SRC_T> lines_processed;
    ap_uint<SRC_T> xindex;
    ap_uint<SRC_T> linebuf[8][COLS] = {0};
    ap_uint<3> line_index;
    bool start;
    bool first_line;
    do
    {
        pixel p = src.read();
        if (p.user)
        {
            lines_processed = 0;
            xindex = 0;
            line_index = 0;
            start = false;
            first_line = true;
        }

        assert(xindex < width);
        linebuf[line_index][xindex] = p.data;
        xindex++;

        if (p.last)
        {
            lines_processed += 1;
            xindex = 0;
            ap_uint<3> line_index0 = (line_index    ) & 0x7;
            ap_uint<3> line_index1 = (line_index + 1) & 0x7;
            ap_uint<3> line_index2 = (line_index + 2) & 0x7;
            ap_uint<3> line_index3 = (line_index - 1) & 0x7;
            ap_uint<3> line_index4 = (line_index - 2) & 0x7;

            box_filter<SRC_T>(linebuf[line_index0],
                              linebuf[line_index1],
                              linebuf[line_index2],
                              linebuf[line_index3],
                              linebuf[line_index4],
                              width,
                              dst,
                              first_line);
            first_line = false;
            line_index = (line_index + 1) & 0x7;
            if (lines_processed >= height)
            {
                eos = true;
            }
        }
    } while (eos == false);
}

#endif // _SIMPLE_HDR_ACCEL_HPP_