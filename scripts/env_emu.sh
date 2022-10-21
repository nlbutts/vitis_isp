. ~/xilinx/Vitis/2022.1/settings64.sh
export ROOTDIR=`git rev-parse --show-toplevel`
export EDGE_COMMON_SW=$ROOTDIR/plinux/images/linux
export DEVICE=$ROOTDIR/platform/jdboard/export/jdboard/jdboard.xpfm
export OPENCV_INCLUDE=~/projects/vitis_opencv/include/opencv4
export OPENCV_LIB=~/projects/vitis_opencv/lib
export LD_LIBRARY_PATH=$OPENCV_LIB:$LD_LIBRARY_PATH


