############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
############################################################
open_project hls_prj
set_top dct
add_files dct.cpp
add_files dct.h
add_files -tb dct-extern.h -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
add_files -tb dct_coeff_table.txt -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
add_files -tb dct_test.cpp -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
add_files -tb in.dat -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
add_files -tb out.golden.dat -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
open_solution "solution_base" -flow_target vitis
set_part {xcvc1802-viva1596-1MP-e-S}
create_clock -period 5 -name default
config_interface -m_axi_alignment_byte_size 64 -m_axi_latency 64 -m_axi_max_widen_bitwidth 512
config_rtl -register_reset_num 3
source "./hls_prj/solution_base/directives.tcl"
csim_design
csynth_design
cosim_design -O -enable_dataflow_profiling
export_design -format ip_catalog
