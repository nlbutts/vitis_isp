/*
 * Copyright 2021 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "xf_isp_types.h"

/*********************************************************************************
 * Function:    ISPPipeline_accel
 * Parameters:  input and output image pointers, image resolution
 * Return:
 * Description:
 **********************************************************************************/
extern "C" {
void RGB2YUV_accel(ap_uint<INPUT_PTR_WIDTH>* img_inp,
                       ap_uint<OUTPUT_PTR_WIDTH>* img_out,
                       int height,
                       int width)
{
// clang-format off
#pragma HLS INTERFACE m_axi     port=img_inp  offset=slave bundle=gmem1
#pragma HLS INTERFACE m_axi     port=img_out  offset=slave bundle=gmem2
// clang-format on
#pragma HLS DATAFLOW
    xf::cv::Mat<XF_8UC3, XF_HEIGHT, XF_WIDTH, XF_NPPC, 3> imgInput(height, width);
    xf::cv::Mat<XF_16UC1, XF_HEIGHT, XF_WIDTH, XF_NPPC, 1> imgOutput(height, width);

    xf::cv::Array2xfMat<INPUT_PTR_WIDTH, XF_8UC3, XF_HEIGHT, XF_WIDTH, XF_NPPC, 3>(img_inp, imgInput);
    xf::cv::rgb2yuyv<XF_LTM_T, XF_16UC1, XF_HEIGHT, XF_WIDTH, XF_NPPC, 3, 1>(
        imgInput, imgOutput);

    xf::cv::xfMat2Array<OUTPUT_PTR_WIDTH, XF_16UC1, XF_HEIGHT, XF_WIDTH, XF_NPPC, 1>(imgOutput, img_out);
}
}
