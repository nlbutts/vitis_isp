. ~/xilinx/Vitis/2022.1/settings64.sh
. /opt/petalinux/2022.1/environment-setup-cortexa72-cortexa53-xilinx-linux
export ROOTDIR=`git rev-parse --show-toplevel`
export EDGE_COMMON_SW=$ROOTDIR/plinux/images/linux
#export DEVICE=$ROOTDIR/platform/jdboard/export/jdboard/jdboard.xpfm
export DEVICE=/home/nlbutts/xilinx/Vitis/2022.1/base_platforms/xilinx_zcu104_base_202210_1/xilinx_zcu104_base_202210_1.xpfm
export SYSROOT=/opt/petalinux/2022.1/sysroots/cortexa72-cortexa53-xilinx-linux
export HOST_ARCH=aarch64
export OPENCV_INCLUDE=/opt/petalinux/2022.1/sysroots/cortexa72-cortexa53-xilinx-linux/include/opencv4
export OPENCV_LIB=/opt/petalinux/2022.1/sysroots/cortexa72-cortexa53-xilinx-linux/lib
export ROOTFS=$ROOTDIR/plinux/images/linux/rootfs.ext4
export K_IMAGE=$ROOTDIR/plinux/images/linux/Image
export TARGET=hw

