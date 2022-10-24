#include <cstdio>

#include "common/xf_headers.hpp"
#include "simple_hdr_accel.hpp"

int main(int argc, char ** argv)
{
    if (argc != 2)
    {
        printf("Usage: simple_hdr <input image>\n");
        return -1;
    }

    auto img = cv::imread(argv[1], -1);
    int rows = img.rows;
    int cols = img.cols;

    cv::Mat blur_img;
    blur_img.create(img.rows, img.cols, CV_8UC3);

    hls::stream<pixel> src;
    hls::stream<pixel> dst;

    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < cols; x++)
        {
            pixel p;
            if ((y == 0) && (x == 0))
                p.user = 1;
            else
                p.user = 0;

            if ((x == (cols - 1)))
                p.last = 1;

            auto tp = img.at<uint16_t>(x, y);
            p.data = tp;
            src.write(p);
        }
    }

    simple_stream<BITS, 128, 128>(src, dst, cols, rows);

    // printf("Dst: ");
    // bool eos = false;
    // do
    // {
    //     pixel p2 = dst.read();
    //     printf("%d ", (int)p2.data);

    //     eos = p2.last;
    // } while (eos == false);

    //cv::imwrite("blur.png", blur_img);
}

