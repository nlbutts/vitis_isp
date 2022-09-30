#!/bin/bash

#set -o xtrace
set -o errexit -o nounset -o pipefail -o errtrace
IFS=$'\n\t'

verify_build_system() {
    echo "Checking"
    if [[ -n `which vitis` && -n `which petalinux-config` ]]; then
        echo "Vitis and petalinux are present, checking version"
        if [[ `vitis -version | grep "Vitis v" | awk '{print $3}'` == "v2022.1" ]]; then
            echo "Correct version of Vitis, checking petalinux"
        else
            echo "Wrong version of Vitis, this code requires v2022.1"
            exit 1
        fi
    else
        echo "You need to source Vitis and Petalinux"
        exit 1
    fi
}

build_xsa() {
    cd jd106_extend
    if [[ $1 -eq 1 ]]; then
        echo "Cleaning Vivado platform"
        rm -rf golden_image
        rm -f *.jou
        rm -f *.log
        rm -rf jd106_extend
        rm *.xsa
    else
        vivado -source jd106_extend.tcl -mode batch
        vivado -source build.tcl -mode batch -project jd106_extend/jd106_extend.xpr
    fi

    cd ..
}


build_linux() {
    cd plinux
    if [[ $1 -eq 1 ]]; then
        echo "Cleaning Linux"
        rm -rf build
    else
        petalinux-config --silentconfig --get-hw-description ../jd106_extend/MPSoC_ext_platform_wrapper.xsa
        petalinux-build --sdk
        petalinux-package --boot --fsbl --fpga --u-boot --pmufw --force
        sudo plinux/images/linux/sdk.sh -y
    fi
    cd ..
}

build_platform() {
    cd platform
    if [[ $1 -eq 1 ]]; then
        echo "Cleaning Platform"
        rm -rf jd106
    else
        xsct build.tcl
    fi
    cd ..
}

build_vitis() {
    cd accel
    export ROOTDIR=`git rev-parse --show-toplevel`
    export EDGE_COMMON_SW=$ROOTDIR/plinux/images/linux
    export DEVICE=$ROOTDIR/platform/jd106/export/jd106/jd106.xpfm
    export SYSROOT=/opt/petalinux/2022.1/sysroots/cortexa72-cortexa53-xilinx-linux
    export HOST_ARCH=aarch64
    export OPENCV_INCLUDE=/opt/petalinux/2022.1/sysroots/cortexa72-cortexa53-xilinx-linux/include/opencv4
    export OPENCV_LIB=/opt/petalinux/2022.1/sysroots/cortexa72-cortexa53-xilinx-linux/lib
    export TARGET=hw

    if [[ $1 -eq 1 ]]; then
        echo "Cleaning Vitis Accelerators"
        make cleanall
    else
        make build
    fi
    cd ..
}

CLEAN=0
if [[ $# -eq 1 ]]; then
    if [[ $1 == "clean" ]]; then
        CLEAN=1
        echo "Setting the clean flag"
    fi
fi

verify_build_system
build_xsa $CLEAN
build_linux $CLEAN
build_platform $CLEAN
build_vitis $CLEAN
