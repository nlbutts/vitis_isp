#include "simple_hdr_config.hpp"
#include "imgproc/xf_gaussian_filter.hpp"


//Hp = GaussianBlur(img) + mean(img) / 2
//Ibip = (Icfa(max) + H(p)) * (Icfa(p) / (Icfa(p) + H(p)))

//Ap = GaussianBlur(Ibip) + mean(Ibip) / 2
//Iga = (Ibip(max) + A(p)) * (Ibip(p) / (Ibip(p) + A(p)))

ap_uint<12> calculate_box(ap_uint<12> * window)
{
    ap_uint<16> sum = 0;
    for (ap_uint<6> i = 0; i < 25; i++)
    {
        sum += window[i];
    }
    sum /= 25;
    return sum;
}

void box_filter(ap_uint<12> *line0,
                ap_uint<12> *line1,
                ap_uint<12> *line2,
                ap_uint<12> *line3,
                ap_uint<12> *line4,
                ap_uint<12> width,
                hls::stream<pixel>& dst,
                bool SOF)
{
    pixel dstp;
    ap_uint<12> window[25] = {0};
    for (ap_uint<12> x = 0; x < width; x++)
    {
        for (ap_uint<3> win = 0; win < 5; win++)
        {
            if (((win - 2) < x) && (((win - 2) + x) >= width))
            {
                window[(win * 5) + 1] = 0;
                window[(win * 5) + 2] = 0;
                window[(win * 5) + 3] = 0;
                window[(win * 5) + 4] = 0;
                window[(win * 5) + 5] = 0;
            }
            else
            {
                window[(win * 5) + 1] = line0[x];
                window[(win * 5) + 2] = line1[x];
                window[(win * 5) + 3] = line2[x];
                window[(win * 5) + 4] = line3[x];
                window[(win * 5) + 5] = line4[x];
            }
        }
        dstp.data = calculate_box(window);
        dstp.user = SOF;
        if (x == (width - 1))
        {
            dstp.last = true;
        }
        dst.write(dstp);
    }
}

void simple_stream(hls::stream<pixel>& src, hls::stream<pixel>& dst, ap_uint<12> width, ap_uint<12> height)
{
    bool eos = false;
    ap_uint<12> lines_processed;
    ap_uint<12> xindex;
    ap_uint<12> linebuf[1928][8] = {0};
    ap_uint<3> line_index;
    bool start;
    do
    {
        pixel p = src.read();
        if (p.user)
        {
            lines_processed = 0;
            xindex = 0;
            line_index = 0;
            start = false;
        }

        assert(xindex < width);
        linebuf[xindex][line_index] = p.data;
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

            box_filter(&linebuf[0][line_index0],
                       &linebuf[0][line_index1],
                       &linebuf[0][line_index2],
                       &linebuf[0][line_index3],
                       &linebuf[0][line_index4],
                       width,
                       dst,
                       p.user);
            line_index = (line_index + 1) & 0x7;
            if (lines_processed >= height)
            {
                eos = true;
            }
        }
    } while (eos == false);
}

