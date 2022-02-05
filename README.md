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

## Build the Vitis DCT hardware project
Run the following, you may need to tweak **env.sh** based on the location of
various components. Refer to **env.sh** for details.

```
. <xilinx install directory>/Vitis/2021.2/settings64.sh
. <vitis_isp checkout directory>/scripts/env.sh
cd <vitis_isp checkout directory>/dct_example
make all TARGET=hw
```

This will take some time, but create a sd_card.img file. You can **dd** this
file onto to an SD card and boot the unit. To run the dct example, do the
following on the booted unit:
```
cd /media/sd-mmcblk0p1
./run_script.sh
```

This will load the xclbin file and run the **dct_example** program.

# Y2K22 bug fix
Xilinx has issues with 2022. See this post for a fix
https://support.xilinx.com/s/article/76960?language=en_US


# Boot faster
To boot faster grab the **pmufw**, **system.bit**, **bl31.elf**, and
**system.dtb** from the Extensible platform design.
The Petalinux build doesn't work for some reason.
Then grab the fsbl and u-boot from the Petalinux build.

Normally I copy :
**pmufw**
**system.bit**
**bl31.elf**
**system.dtb**

into the <vitis_isp directory>/plinux/images/linux directory, overwritting
the petalinux builds.

Then I run the following:
```
petalinux-package --boot --u-boot --fpga system.bit --force
```

This will create a new BOOT.BIN file that you can copy to the SD card.

Next I create a new boot.scr file:

```
# Generate boot.scr:
# mkimage -c none -A arm -T script -d boot.txt boot.scr
#
################
#setenv bootargs "rootwait root=/dev/mmcblk0p2 quiet systemd.show_status=0"
#setenv bootargs "earlycon console=ttyPS0,115200 clk_ignore_unused root=/dev/mmcblk0p2 rw rootwait"

fatload mmc 0 0x01000000 Image.gz
unzip 0x01000000 0x00200000
booti 0x00200000 - 0x00100000
```

I then compress Image using gzip and copy it to the boot partition on the SD
card.
```
gzip -k Image
```

Put the SD card into the unit and boot. Stop at u-boot and do the following:
```
fatload mmc 0 0x80000 BOOT.BIN
sf probe
sf update 0x80000 0 ${filesize}
fatload mmc 0 0x80000 boot.scr
sf update 0x80000 0x2000000 ${filesize}
```

Shut the unit off, configure it to boot from QSPI. Then boot. Loading the
BIT file, u-boot and the kernel should only take a few seconds now.

When u-boot starts up change the bootargs to:
```
setenv bootargs "earlycon console=ttyPS0,115200 clk_ignore_unused root=/dev/mmcblk0p2 rw rootwait quiet systemd.show_status=0"
saveenv
```

You can also create a script to do this update:
```
setenv update_boot "fatload mmc 0 0x80000 BOOT.BIN;sf probe; sf update 0x80000 0 ${filesize};fatload mmc 0 0x80000 boot.scr;sf update 0x80000 0x2000000 ${filesize}"
```

The unit should boot within 15-20 seconds.