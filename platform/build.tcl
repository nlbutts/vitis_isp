#
# Usage: To re-create this platform project launch xsct with below options.
# xsct /home/nlbutts/projects/vitis_isp/platform/MPSoC_ext_platform_wrapper/platform.tcl
#
# OR launch xsct and run below command.
# source /home/nlbutts/projects/vitis_isp/platform/MPSoC_ext_platform_wrapper/platform.tcl
#
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {jdboard}\
-hw {../board/MPSoC_ext_platform_wrapper.xsa}\
-proc {psu_cortexa53} -os {linux} -arch {64-bit} -fsbl-target {psu_cortexa53_0} -out {.}

platform write
platform active {jdboard}
domain config -display-name {xrt}
platform write
domain config -generate-bif
platform write
domain config -boot {../plinux/images/linux}
platform write
platform generate

createdts -hw ../board/MPSoC_ext_platform_wrapper.xsa -out . -zocl -platform-name jdboard\
 -git-branch xlnx_rel_v2022.1 -compile
