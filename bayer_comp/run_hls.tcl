#
# Copyright 2019-2021 Xilinx, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

source settings.tcl

set PROJ "bayer_comp.prj"
set SOLN "sol1"
set PRJROOT $env(PRJROOT)
set VITIS_LIB /home/nlbutts/projects/vitis_isp/Vitis_Libraries/vision

if {![info exists CLKP]} {
  set CLKP 3.3
}

open_project -reset $PROJ

add_files "bayer_comp_accel.cpp" -cflags " -I ${PRJROOT}/build -I${VITIS_LIB}/L1/include -I ./ -D__SDSVHLS__ -std=c++14" -csimflags " -I ${PRJROOT}/build -I${VITIS_LIB}/L1/include -I ./ -D__SDSVHLS__ -std=c++14"
add_files -tb "bayer_comp_tb.cpp" -cflags " -I ${PRJROOT}/build -I ${OPENCV_INCLUDE} -I${VITIS_LIB}/L1/include -I ./ -D__SDSVHLS__ -std=c++14" -csimflags " -I ${PRJROOT}/build -I${VITIS_LIB}/L1/include -I ./ -D__SDSVHLS__ -std=c++14"
set_top bayer_comp_accel

open_solution -reset $SOLN




set_part $XPART
create_clock -period $CLKP

if {$CSIM == 1} {
  csim_design -ldflags "-L ${OPENCV_LIB} -lopencv_imgcodecs -lopencv_imgproc -lopencv_core -lopencv_highgui -lopencv_flann -lopencv_features2d" -argv " ${PRJROOT}/test.png"
}

if {$CSYNTH == 1} {
  csynth_design
}

if {$COSIM == 1} {
  cosim_design -ldflags "-L ${OPENCV_LIB} -lopencv_imgcodecs -lopencv_imgproc -lopencv_core -lopencv_highgui -lopencv_flann -lopencv_features2d" -argv " ${PRJROOT}/test.png"
}

if {$VIVADO_SYN == 1} {
  export_design -flow syn -rtl verilog
}

if {$VIVADO_IMPL == 1} {
  export_design -flow impl -rtl verilog
}

exit