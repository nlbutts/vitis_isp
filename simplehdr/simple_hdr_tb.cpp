#include <cstdio>

#include "common/xf_headers.hpp"
#include "simple_hdr_config.hpp"

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

    //simple_hdr((ap_uint<INPUT_PTR_WIDTH>*)img.data, (ap_uint<OUTPUT_PTR_WIDTH>*)blur_img.data, rows, cols, 1, 1, 3, 1.5);
    ap_uint<16> result;
    div_test(1234, 512, 12, &result);

    printf("Resulting pixel: %d\n", (int)result);

    //cv::imwrite("blur.png", blur_img);
}

