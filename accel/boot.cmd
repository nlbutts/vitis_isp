# Generate boot.scr:
# mkimage -c none -A arm -T script -d boot.cmd boot.scr
#
################
fatload mmc 0 ${loadaddr} Image
fatload mmc 0 ${fdt_addr_r} system.dtb
setenv bootargs earlycon console=ttyPS0,115200 clk_ignore_unused root=/dev/mmcblk0p2 rw rootwait
booti ${loadaddr} - ${fdt_addr_r}