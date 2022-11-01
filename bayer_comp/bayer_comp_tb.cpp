#include <cstdio>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs/imgcodecs.hpp"

#include "rice.h"
#include <boost/crc.hpp>
#include "bayer_comp_struct.h"
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

void append_comp_channel(std::vector<uint8_t> &pkg, uint8_t * comp_data, int comp_size)
{
    for (int i = 0; i < comp_size; i++)
    {
        pkg.push_back(comp_data[i]);
    }
}

template<typename T>
void save_vector(std::string filename, std::vector<T> &pkg)
{
    printf("Saving vector to file %s\n", filename.c_str());
    FILE * f = fopen(filename.c_str(), "wb");
    fwrite(pkg.data(), sizeof(pkg[0]), pkg.size(), f);
    fclose(f);
}


std::vector<uint8_t> compress_ref(cv::Mat &img)
{
    int total_pixels = img.rows * img.cols;
    std::vector<int16_t> channels[4];
    uint8_t * comp_data[4];
    comp_data[0] = (uint8_t*)malloc(total_pixels/2);
    comp_data[1] = (uint8_t*)malloc(total_pixels/2);
    comp_data[2] = (uint8_t*)malloc(total_pixels/2);
    comp_data[3] = (uint8_t*)malloc(total_pixels/2);

    // Split the bayer image into RGGB planes
    uint16_t prev_pixel[4];
    for (int y = 0; y < img.rows; y++)
    {
        for (int x = 0; x < img.cols; x++)
        {
            int index = ((y & 1) << 1) + (x & 1);
            uint16_t pixel = img.at<uint16_t>(y, x);
            if (((y == 0) && (x <= 1)) || ((y == 1) && (x <= 1)))
            {
                channels[index].push_back(pixel);
                prev_pixel[index] = pixel;
            }
            else
            {
                channels[index].push_back(pixel - prev_pixel[index]);
                prev_pixel[index] = pixel;
            }
        }
    }

    // Now compress
    int comp_size[4];
    for (int ch = 0; ch < 4; ch++)
    {
        comp_size[ch] = Rice_Compress((void*)channels[ch].data(),
                                      comp_data[ch],
                                      channels[ch].size() * 2,
                                      RICE_FMT_INT16);
        printf("data: %08X  size: %d  comp_size: %d\n",
                channels[ch].data(),
                channels[ch].size(),
                comp_size[ch]);
    }

    BayerComp header;
    header.type[0] = 'C';
    header.type[1] = 'F';
    header.type[2] = 'A';
    header.type[3] = 'C';
    header.type[4] = '0';
    header.type[5] = '0';
    header.type[6] = '1';
    header.channels = 4;
    boost::crc_32_type crc;
    crc.process_bytes(img.data, total_pixels * 2);
    header.crc = crc();
    header.width = img.cols;
    header.height = img.rows;
    header.channel_size[0] = comp_size[0];
    header.channel_size[1] = comp_size[1];
    header.channel_size[2] = comp_size[2];
    header.channel_size[3] = comp_size[3];
    auto src = (uint8_t*)&header;
    std::vector<uint8_t> pkg(src, src + sizeof(header));

    append_comp_channel(pkg, comp_data[0], comp_size[0]);
    append_comp_channel(pkg, comp_data[1], comp_size[1]);
    append_comp_channel(pkg, comp_data[2], comp_size[2]);
    append_comp_channel(pkg, comp_data[3], comp_size[3]);

    save_vector<uint8_t>("comp.cfa", pkg);

    free(comp_data[0]);
    free(comp_data[1]);
    free(comp_data[2]);
    free(comp_data[3]);

    return pkg;
}

bool verifyHeader(BayerComp * header)
{
    bool result = false;
    if ((header->type[0] == 'C') &&
        (header->type[1] == 'F') &&
        (header->type[2] == 'A') &&
        (header->type[3] == 'C') &&
        (header->type[4] == '0') &&
        (header->type[5] == '0') &&
        (header->type[6] == '1') &&
        (header->channels == 4))
    {
        result = true;
        printf("Header is good\n");
    }
    else
    {
        printf("Header is bad\n");
    }

    return result;
}

void diff_to_uint16(int16_t * data, int size)
{
    for (int i = 1; i < (size/2); i++)
    {
        data[i] = data[i - 1] + data[i];
    }
}

cv::Mat decompress_ref(std::vector<uint8_t> &comp_data, std::string filename)
{
    cv::Mat img;
    BayerComp * header = (BayerComp *)comp_data.data();

    if (verifyHeader(header))
    {
        auto src = comp_data.data() + sizeof(BayerComp);
        int16_t * decomp_data[4];
        int uncompressed_size = header->width * header->height * 2 / 4;
        for (int i = 0; i < header->channels; i++)
        {
            decomp_data[i] = (int16_t*)malloc(uncompressed_size);
            Rice_Uncompress(src,
                            decomp_data[i],
                            header->channel_size[i],
                            uncompressed_size,
                            RICE_FMT_INT16);
            diff_to_uint16(decomp_data[i], uncompressed_size);
            src += header->channel_size[i];
            // std::vector<uint16_t> test(decomp_data[i], decomp_data[i] + uncompressed_size/2);
            // std::stringstream ss;
            // ss << "test";
            // ss << i;
            // ss << ".bin";
            // save_vector<uint16_t>(ss.str(), test);
        }

        printf("Assembling image\n");
        img.create(header->height, header->width, CV_16UC1);

        uint16_t * ptr = (uint16_t*)img.data;
        int offset = header->width / 2;
        for (int y = 0; y < header->height; y++)
        {
            for (int x = 0; x < header->width; x++)
            {
                int index = ((y & 1) << 1) + (x & 1);
                *ptr = decomp_data[index][((y >> 1) * offset) + (x >> 1)];
                ptr++;
            }
        }

        cv::imwrite("ref.png", img);
    }
    return img;
}

int compare_images(cv::Mat img1, cv::Mat img2)
{
    int result = -1;
    cv::Mat diff;
    cv::subtract(img1, img2, diff);
    auto sum = cv::sum(diff);
    printf("Sum of diff: %f\n", sum);
    if ((-0.1 < sum.val[0]) && (sum.val[0] < 0.1))
    {
        result = 0;
    }
    return result;
}

int main(int argc, char ** argv)
{
    int result = 0;

    if (argc != 2)
    {
        printf("Usage: simple_hdr <input image>\n");
        return -1;
    }

    int size;
    cv::Mat img = cv::imread(argv[1], -1);

    if (!img.empty())
    {
        auto comp_data = compress_ref(img);
        auto ref = decompress_ref(comp_data, "ref.png");
        result = compare_images(img, ref);
    }
    else
    {
        printf("ERROR: Can't open file\n");
        result = -1;
    }

    // hls::stream<int16_t> indata;
    // hls::stream<uint8_t> outdata;
    // size = 100;
    // for (int i = 0; i < size; i++)
    // {
    //     indata.write(data[i]);
    // }

    // //int compsize = Rice_Compress_accel((int16_t*)data, compdata, size, 7);
    // int compsize = Rice_Compress_accel(indata, outdata, size, 7);

    // for (int i = 0; i < compsize; i++)
    // {
    //     compdata[i] = outdata.read();
    // }

    // printf("Input size: %d  output size: %d\n", size, compsize);

    // FILE * f = fopen("comp.rice", "wb");
    // fwrite(compdata, 1, compsize, f);
    // fclose(f);

    // uint8_t * golden = (uint8_t*)open_file(argv[2], &size);
    // uint8_t * ptr = &golden[0x0d];
    // int result = memcmp(ptr, compdata, compsize - 1);
    // printf("Memory compare result: %d\n", result);

    return result;

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

