#include <cstdio>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utils/logger.hpp>

#include <chrono>

void print_stats(std::string s, cv::Mat &img)
{
    double minv, maxv;
    cv::Point minidx, maxidx;
    cv::minMaxLoc(img, &minv, &maxv, &minidx, &maxidx);
    printf("%s Minv: %f  Maxv: %f\n", s.c_str(), minv, maxv);
}

void hdr_float(cv::Mat img, float k1, float k2)
{
    cv::Mat Hp, Ap, Ibip, Iga, out_img, num, den, temp1, fimg, Iga_fixed, bgr;
    cv::Scalar mean;
    int rows = img.rows;
    int cols = img.cols;
    int depth = img.depth();

    double minv;
    double maxv;
    cv::Point pt1, pt2;
    cv::minMaxLoc(img, &minv, &maxv, &pt1, &pt2);
    float scale = 1 / maxv;
    img.convertTo(fimg, CV_32F, scale);

    Hp.create(img.rows, img.cols, CV_32F);
    Ap.create(img.rows, img.cols, CV_32F);
    temp1.create(img.rows, img.cols, CV_32F);
    num.create(img.rows, img.cols, CV_32F);
    den.create(img.rows, img.cols, CV_32F);
    Ibip.create(img.rows, img.cols, CV_32F);
    Iga.create(img.rows, img.cols, CV_32F);
    out_img.create(img.rows, img.cols, CV_32F);

    //Hp = GaussianBlur(img) + mean(img) / 2
    //Ibip = (Icfa(max) + H(p)) * (Icfa(p) / (Icfa(p) + H(p)))

    //Ap = GaussianBlur(Ibip) + mean(Ibip) / 2
    //Iga = (Ibip(max) + A(p)) * (Ibip(p) / (Ibip(p) + A(p)))

    printf("Input image WxH: %dx%d  Depth: %d\n", cols, rows, depth);

    auto start = std::chrono::high_resolution_clock::now();

    cv::Scalar k1s = k1;
    cv::Scalar k2s = k2;

    for (int i = 0; i < 10; i++)
    {
        cv::GaussianBlur(fimg, Hp, cv::Size(5, 5), 3);
        //mean = cv::mean(fimg) / 2;
        cv::add(Hp, k1s, Hp);
        //cv::imwrite("Hp.png", Hp);
        cv::add(fimg, Hp, den, cv::noArray());
        //cv::imwrite("temp1.png", temp1);
        //print_stats("den", den);
        cv::add(Hp, 1.0f, temp1);
        cv::divide(fimg, den, num);
        cv::multiply(temp1, num, Ibip);
        // print_stats("Hp", Hp);
        // print_stats("fimg", fimg);
        // print_stats("ibip", Ibip);
        //cv::imwrite("ibip.png", Ibip);

        //cv::minMaxLoc(Ibip, &minv, &maxv, &pt1, &pt2);
        cv::GaussianBlur(Ibip, Ap, cv::Size(5, 5), 1.5);
        //mean = cv::mean(Ibip) / 2;
        cv::add(Ap, k2s, Ap);
        //cv::imwrite("Ap.png", Ap);
        cv::add(Ibip, Ap, den, cv::noArray());
        //cv::imwrite("temp1.png", temp1);
        //print_stats("temp1", temp1);
        cv::add(Ap, 1.0f, temp1);
        cv::divide(Ibip, den, num);
        cv::multiply(temp1, num, Iga);
        //print_stats("Iga", Iga);
    }

    printf("Getting stop time\n");
    auto stop = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration<double>(stop - start);
    diff /= 10;
    printf("Time taken: %f\n", diff);

    //print_stats("Iga", Iga);
    cv::minMaxLoc(img, &minv, &maxv, &pt1, &pt2);
    Iga.convertTo(Iga_fixed, CV_8U, 255, 0);
    print_stats("Iga_fixed", Iga_fixed);

    cv::imwrite("Iga_float.png", Iga_fixed);

    //cv::cvtColor(Iga_fixed, bgr, cv::COLOR_BayerBG2BGR);
    //cv::cvtColor(Iga_fixed, bgr, cv::COLOR_BayerRG2BGR);
    cv::cvtColor(Iga_fixed, bgr, cv::COLOR_BayerGB2BGR);
    cv::imwrite("out.png", bgr);
}

void hdr_fixed(cv::Mat img)
{
    cv::Mat Hp, Ap, Ibip, Iga, out_img, temp1, temp2, fimg;

    int rows = img.rows;
    int cols = img.cols;
    int depth = img.depth();

    Hp.create(img.rows, img.cols, CV_16U);
    Ap.create(img.rows, img.cols, CV_16UC1);
    temp1.create(img.rows, img.cols, CV_16UC1);
    temp2.create(img.rows, img.cols, CV_32S);
    Ibip.create(img.rows, img.cols, CV_16UC1);
    Iga.create(img.rows, img.cols, CV_16UC1);
    out_img.create(img.rows, img.cols, CV_8UC3);

    //Ibip = (Icfa(max) + H(p)) * (Icfa(p) / (Icfa(p) + H(p)))
    //Iga = (Ibip(max) + A(p)) * (Ibip(p) / (Ibip(p) + A(p)))

    //printf("OpenCV Version: %s\n", cv::getBuildInformation().c_str());
    printf("Input image WxH: %dx%d  Depth: %d\n", cols, rows, depth);
    printf("Hp: Depth: %d\n", Hp.depth());

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 10; i++)
    {
        //cv::GaussianBlur(img, Hp, cv::Size(5, 5), 3);
        cv::blur(img, Hp, cv::Size(5, 5), cv::Point(-1,-1), 0);
        //cv::imwrite("Hp.png", Hp);
        cv::add(img, Hp, temp1, cv::noArray());
        //cv::imwrite("temp1.png", temp1);
        //print_stats("temp1", temp1);
        cv::multiply(Hp, img, temp2, 1, CV_32S);
        //print_stats("Hp", Hp);
        //print_stats("img", img);
        //print_stats("temp2", temp2);
        cv::divide(temp2, temp1, Ibip, 1, CV_16UC1);
        //print_stats("ibip", Ibip);
        //cv::imwrite("ibip.png", Ibip);

        //cv::GaussianBlur(Ibip, Ap, cv::Size(5, 5), 1.5, 1.5);
        cv::blur(Ibip, Ap, cv::Size(5,5));
        //cv::imwrite("Ap.png", Ap);
        cv::add(Ibip, Ap, temp1, cv::noArray());
        //cv::imwrite("temp1.png", temp1);
        //print_stats("temp1", temp1);
        cv::multiply(Ap, Ibip, temp2, 1, CV_32S);
        //print_stats("Hp", Hp);
        //print_stats("img", img);
        //print_stats("temp2", temp2);
        cv::divide(temp2, temp1, Iga, 1, CV_16UC1);

    }

    printf("Getting stop time\n");
    auto stop = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration<double>(stop - start);
    diff /= 10;
    printf("Time taken: %f\n", diff);

    //print_stats("Iga", Iga);
    cv::imwrite("Iga_fixed.png", Iga);
}

int main(int argc, char ** argv)
{
    if (argc != 4)
    {
        printf("Usage: simple_hdr <input image> <k1> <k2>\n");
        return -1;
    }

    cv::Mat img;
    img = cv::imread(argv[1], -1);

    //printf("OpenCV Version: %s\n", cv::getBuildInformation().c_str());

    float k1, k2;
    sscanf(argv[2], "%f", &k1);
    sscanf(argv[3], "%f", &k2);

    printf("Running floating point version with k1: %f  k2: %f\n", k1, k2);
    hdr_float(img, k1, k2);
    // printf("Running fixed point version\n");
    // hdr_fixed(img);
}

