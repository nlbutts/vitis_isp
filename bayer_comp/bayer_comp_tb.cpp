#include <cstdio>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string.h>

#include "bayer_comp_accel.hpp"

void * open_file(const char * file, int * size)
{
    void * data = NULL;
    FILE * f = fopen(file, "rb");
    if (f != NULL)
    {
        fseek(f, 0, SEEK_END);
        *size = ftell(f);
        data = malloc((*size) + 1000);
        fseek(f, 0, SEEK_SET);
        fread(data, 1, *size, f);
        fclose(f);
    }
    else
    {
        printf("File was not opened or read\n");
    }
    return data;
}

int main(int argc, char ** argv)
{
    if (argc != 3)
    {
        printf("Usage: simple_hdr <input image> <compare image>\n");
        return -1;
    }

    int size;
    int16_t * data = (int16_t*)open_file(argv[1], &size);
    uint8_t * compdata = (uint8_t*)malloc(size);

    hls::stream<int16_t> indata;
    hls::stream<uint8_t> outdata;
    size = 100;
    for (int i = 0; i < size; i++)
    {
        indata.write(data[i]);
    }

    //int compsize = Rice_Compress_accel((int16_t*)data, compdata, size, 7);
    int compsize = Rice_Compress_accel(indata, outdata, size, 7);

    for (int i = 0; i < compsize; i++)
    {
        compdata[i] = outdata.read();
    }

    printf("Input size: %d  output size: %d\n", size, compsize);

    FILE * f = fopen("comp.rice", "wb");
    fwrite(compdata, 1, compsize, f);
    fclose(f);

    uint8_t * golden = (uint8_t*)open_file(argv[2], &size);
    uint8_t * ptr = &golden[0x0d];
    int result = memcmp(ptr, compdata, compsize);
    printf("Memory compare result: %d\n", result);

    free(compdata);
    free(data);
    free(golden);

    //return result;
    return 0;

#if 0
    auto img = cv::imread(argv[1], -1);
    int rows = img.rows;
    int cols = img.cols;

    cv::Mat blur_img;
    blur_img.create(img.rows, img.cols, CV_8UC3);

    hls::stream<pixel> src;
    hls::stream<pixel> dst[4];

    std::vector<uint16_t> output[4];

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

            auto tp = img.at<uint16_t>(y, x);
            if ((y == 0) && (x < 10))
            {
                printf("%04X ", (int)tp);
            }
            p.data = tp;
            src.write(p);
        }
    }

    printf("\n");

    bayer_comp_accel(src, dst[0], dst[1], dst[2], dst[3], cols, rows);

    int last_count = 0;
    int debug = 0;
    do
    {
        for (int i = 0; i < 4; i++)
        {
            bool last = false;
            printf("i: %d\n", i);
            debug = 0;
            do
            {
                pixel p2 = dst[i].read();
                output[i].push_back(p2.data);
                last = p2.last;
                if (p2.last)
                {
                    last_count++;
                    printf("i: %d p2.last: %d\n", i, (int)p2.last);
                }

                if (debug < 10)
                {
                    debug++;
                    printf("%04X ", (int)p2.data);
                }
            } while (!last);

        }
    } while (last_count < 4);

    for (int i = 0; i < 4; i++)
    {
        std::stringstream ss;
        ss << "diff" << i << ".bin";
        FILE * f = fopen(ss.str().c_str(), "wb");
        fwrite(output[i].data(), 2, output[i].size(), f);
        fclose(f);
    }
#endif
}

