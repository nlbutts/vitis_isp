. ~/xilinx/Vitis/2022.1/settings64.sh
export PRJROOT=`pwd`
export ROOTDIR=`git rev-parse --show-toplevel`
export DEVICE=$XILINX_VITIS/base_platforms/xilinx_zcu104_base_202210_1/xilinx_zcu104_base_202210_1.xpfm
export OPENCV_INCLUDE=$PRJROOT/ocvbuild/vitis_opencv/include/opencv4
export OPENCV_LIB=$PRJROOT/ocvbuild/vitis_opencv/lib
export LD_LIBRARY_PATH=$OPENCV_LIB:$LD_LIBRARY_PATH


