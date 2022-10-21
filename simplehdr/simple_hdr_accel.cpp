#include "simple_hdr_config.hpp"

/*
K1 = (np.mean(img) / 2).astype('uint16')
#K1 = 5
Hp = cv.GaussianBlur(img, (3,3), 3) + K1
temp1 = img + Hp
print_stats(1, temp1)
temp2 = ((img.astype('uint32') * 4092) / temp1)
print_stats(2, temp2)
temp2 = temp2.astype('uint32')
temp2 += 1

Ibip = (Hp.astype('uint32') * temp2.astype('uint32')) / 4096
#Ibip = (Hp * ((img * 4092) / (img + Hp))) / 4092
print_stats(3, Ibip)
Ibip = Ibip.astype('uint16')
# Ibip = (Icfa(max) + H(p)) * (Icfa(p) / (Icfa(p) + H(p)))

#plt.imshow(Ibip, cmap='gray')

K2 = (np.mean(Ibip) / 2).astype('uint16')
#K2 = 1
As = cv.GaussianBlur(Ibip, (3,3), 1.5) + K2
print_stats(5, As)

Ibip = Ibip.astype('uint32')
As = As.astype('uint32')
temp4 = Ibip + As
temp4 += 1
print_stats(6, temp4)
temp5 = (Ibip * 4092) / temp4
print_stats(7, temp5)
temp5 = temp5.astype('uint32')
Iga = ((As * temp5) / (4096 * 4.5))
print_stats(8, Iga)
print(f'K1: {K1}  K2: {K2}')
#Iga = (Iga * 255).astype('uint8')
Iga = Iga.astype('uint8')

plt.imshow(Iga, cmap='gray')
*/

static constexpr int __XF_DEPTH = (HEIGHT * WIDTH * (XF_PIXELWIDTH(TYPE, NPC1)) / 8) / (INPUT_PTR_WIDTH / 8);

void simple_hdr(ap_uint<INPUT_PTR_WIDTH>* img_inp, ap_uint<OUTPUT_PTR_WIDTH>* img_out, int rows, int cols, int K1, int K2, int sigma1, int sigma2)
{
// clang-format off
    #pragma HLS INTERFACE m_axi     port=img_inp  offset=slave bundle=gmem1 depth=__XF_DEPTH
    #pragma HLS INTERFACE m_axi     port=img_out  offset=slave bundle=gmem2 depth=__XF_DEPTH
    #pragma HLS INTERFACE s_axilite port=rows
    #pragma HLS INTERFACE s_axilite port=cols
    #pragma HLS INTERFACE s_axilite port=K1
    #pragma HLS INTERFACE s_axilite port=K2
    #pragma HLS INTERFACE s_axilite port=sigma1
    #pragma HLS INTERFACE s_axilite port=sigma2
    // clang-format on

    xf::cv::Mat<TYPE, HEIGHT, WIDTH, NPC1, XF_CV_DEPTH_IN_1> in_mat(rows, cols);
    xf::cv::Mat<TYPE, HEIGHT, WIDTH, NPC1, XF_CV_DEPTH_OUT_1> den1(rows, cols);
    xf::cv::Mat<TYPE, HEIGHT, WIDTH, NPC1, XF_CV_DEPTH_OUT_1> blur1(rows, cols);
    xf::cv::Mat<TYPE, HEIGHT, WIDTH, NPC1, XF_CV_DEPTH_OUT_1> accum1(rows, cols);
// clang-format off
    #pragma HLS DATAFLOW
    // clang-format on

    //Ibip = (Icfa(max) + H(p)) * (Icfa(p) / (Icfa(p) + H(p)))
    //Iga = (Ibip(max) + A(p)) * (Ibip(p) / (Ibip(p) + A(p)))


    xf::cv::Array2xfMat<INPUT_PTR_WIDTH, TYPE, HEIGHT, WIDTH, NPC1, XF_CV_DEPTH_IN_1>(img_inp, in_mat);

    xf::cv::GaussianBlur<FILTER_WIDTH, XF_BORDER_CONSTANT, TYPE, HEIGHT, WIDTH, NPC1, XF_CV_DEPTH_IN_1,
                         XF_CV_DEPTH_OUT_1>(in_mat, blur1, sigma1);

    //xf::cv::Accumulate<TYPE, TYPE, HEIGHT, WIDTH, NPC1, XF_CV_DEPTH_OUT_1, XF_CV_DEPTH_OUT_1>(in_mat, blur1, accum1);

    xf::cv::xfMat2Array<OUTPUT_PTR_WIDTH, TYPE, HEIGHT, WIDTH, NPC1, XF_CV_DEPTH_OUT_1>(blur1, img_out);
}

void div_test(ap_uint<16> pixel1, ap_uint<16> pixel2, ap_uint<8> shift, ap_uint<16> *result)
{
    ap_uint<32> temp = pixel1;
    temp <<= shift;
    ap_uint<32> div = temp / pixel2;
    *result = div;
}
