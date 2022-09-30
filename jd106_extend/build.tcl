update_compile_order -fileset sources_1
reset_runs synth_1
launch_runs synth_1 -jobs 24
wait_on_run synth_1
launch_runs impl_1 -to_step write_bitstream -jobs 24 -force
wait_on_run impl_1
write_hw_platform -fixed -include_bit -force -file golden_image_wrapper.xsa