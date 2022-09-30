#!/bin/bash

#set -o xtrace
set -o errexit -o nounset -o pipefail -o errtrace
IFS=$'\n\t'

VIVADO_VERSION=`vivado -version | grep Vivado | awk '{print $2}'`

build_xsa() {
    cd jd106_extend
    if [[ $1 -eq 1 ]]; then
        echo "Cleaning Vivado platform"
        rm -rf golden_image
        rm -f *.jou
        rm -f *.log
        rm -rf jd106_extend
    elif [[ $VIVADO_VERSION == "v2022.1" ]]; then
        echo "Correct version of Vivado, building project"
        vivado -source jd106_extend.tcl -mode batch
        vivado -source build.tcl -mode batch -project jd106_extend/jd106_extend.xpr
    else
        echo "Please install Vivado v2022.1 and source vivado"
        echo "source <install directory>/Vivado/2022.1/settings64.sh"
    fi

    cd ..
}


build_linux() {
    cd plinux
    if [[ $1 -eq 1 ]]; then
        echo "Cleaning Linux"
        rm -rf build
    else
        petalinux-config --silentconfig --get-hw-description ../jd106_extend/golden_image_wrapper.xsa
        petalinux-build --sdk
        petalinux-package --boot --fsbl --fpga --u-boot --pmufw --force
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

CLEAN=0
if [[ $# -eq 1 ]]; then
    if [[ $1 == "clean" ]]; then
        CLEAN=1
        echo "Setting the clean flag"
    fi
fi

#build_xsa $CLEAN
build_linux $CLEAN
#build_platform $CLEAN
