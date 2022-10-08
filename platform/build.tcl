#
# Usage: To re-create this platform project launch xsct with below options.
# xsct /home/nlbutts/projects/vitis_isp/platform/MPSoC_ext_platform_wrapper/platform.tcl
#
# OR launch xsct and run below command.
# source /home/nlbutts/projects/vitis_isp/platform/MPSoC_ext_platform_wrapper/platform.tcl
#
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

createdts -hw ../board/MPSoC_ext_platform_wrapper.xsa -out . -zocl -platform-name jdboard -git-branch xlnx_rel_v2022.1 -compile

# platform create -name {jd106}\
# -hw {/home/nlbutts/projects/vitis_isp/jd106_extend/MPSoC_ext_platform_wrapper.xsa}\
# -arch {64-bit} -fsbl-target {psu_cortexa53_0} -out .
#
# platform write
# domain create -name {standalone_psu_cortexa53_0} -display-name {standalone_psu_cortexa53_0} -os {standalone} -proc {psu_cortexa53_0} -runtime {cpp} -arch {64-bit} -support-app {zynqmp_fsbl}
# platform generate -domains
# platform active {jd106}
# domain active {zynqmp_fsbl}
# domain active {zynqmp_pmufw}
# domain active {standalone_psu_cortexa53_0}
# platform generate -quick
# platform generate
