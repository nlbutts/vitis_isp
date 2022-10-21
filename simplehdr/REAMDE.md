# Simple HDR

This is a HLS design for TBD paper.

## Setup
Xilinx goes out of their way to make life difficult. Using the HLS tools
requires an ANCIENT compiler. But if you source the HLS envionment, then
you will get an ANCIENT cmake version, which will not work for OpenCV.

Oh, and you need a custom build of OpenCV for the ANCIENT compiler. It is
ridiculous. So.. You need to build OpenCV with the old compiler.

Define the following environment variable:
`
export CXX=<vitis HLS dir>/tps/lnx64/gcc-8.3.0/bin/g++
export LIBRARY_PATH=/usr/lib/x86_64-linux-gnu/:$LIBRARY_PATH
`

DO NOT SOURCE VITIS_HLS IT WILL BREAK EVERYTHING

Now run:
`make opencv`

Now you can source the setup environment
`. env.sh`

## Build/SIM
To build the C Simulation run:
`make run CSIM=1`
