# vitis_isp
Vitis ISP Tests

This repo has a simple DCT test showing the GUI flow for HLS development.

I then took the isppipeline and made it into a HLS sim so a person could
quickly develop on the host. You can go to <vitis_library>/vision/L3/examples/isppipeline
and run their make file to re-create the ISP on hardware. There is a handy
script to source all of the environment variables.

# Build
Install Vitis and Petalilnux (see Xilinx docs)

Checkout this repo.

## Build Linux
Source the petalinux **settings.sh** file. In my installation it is:
`
. ~/xilinx/Petalinux/2021.2/settings.sh
cd <vitis_isp>/plinux
petalinux-build
petalinuxbuild -s
cd images/linux
./sdk.sh
`

This will take some time. But it will build the Linux distro, SDK, and install the SDK.

## Build DCT demo
Open Vitis_HLS tool and load the dct project.


# Y2K22 bug fix
Xilinx has issues with 2022. See this post for a fix
https://support.xilinx.com/s/article/76960?language=en_US


# Get Linux working
Use **dd** to copy the sd_card.img to an SD card. But then it won't boot and it
takes forever. The **BOOT.BIN** file that is created has the **.bit** embedded.
The FSBL takes a good long forever to load that. So it is better to take the
output of the plinux build, without the .bit file and then have u-boot load the
FPGA image.

Copy the plinux **BOOT.BIN** into the first partition of the SD card.

`
setenv loadfpga "fatload mmc 0 '${ramdisk_addr_r}' system.bit;fpga load 0 '${ramdisk_addr_r}' '${filesize}'"
saveenv
`