update_compile_order -fileset sources_1
reset_runs synth_1
launch_runs synth_1 -jobs 8
#wait_on_run synth_1
#launch_runs impl_1 -to_step write_bitstream -jobs 24 -force
#wait_on_run impl_1
#write_hw_platform -fixed -include_bit -force -file golden_image_wrapper.xsa

set_property platform.vendor {JD} [current_project]
set_property platform.board_id {jdboard} [current_project]
set_property platform.name {jdboard} [current_project]
#set_property pfm_name {JD:jdboard:jdboard:1.0} [get_files -all {/home/nlbutts/projects/vitis_isp/jd106_extend/jd106_extend/jd106_extend.srcs/sources_1/bd/MPSoC_ext_platform/MPSoC_ext_platform.bd}]
set_property platform.extensible {true} [current_project]
set_property platform.description {Vitis design for jdboard board} [current_project]
set_property platform.uses_pr {false} [current_project]
write_hw_platform -force -file MPSoC_ext_platform_wrapper.xsa