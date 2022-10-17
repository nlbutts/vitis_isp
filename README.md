# vitis_isp
Vitis ISP Tests

This repo attempts to demostrate a complete development pipeline for Vitis
accelerated flows.

# Build
Install Vitis and Petalilnux (see Xilinx docs)

Checkout this repo.

## Source the tools
Source Vitis and Petalinux:
`. <install dir>/Vitis/2022.1/settings64.sh`
`. <install dir>/Petalinux/2022.1/settings.sh`

## Configure for your board
By default the makefile will use the ZCU106 board. But you can override
it by:
`export BOARD=<board name>`

To create new boards you need to have a tcl script that can re-create the board
design. See the **board** directory for examples

## Build the Board
`make board`

This will create the Vivado XSA board file. It does NOT generate a bitstream

## Build Linux
`make linux`

Will pull in the XSA file and build the main Linux distro. This will take
some time depending on the speed of your computer.

## Build the SDK
`make sdk`

This will build the Linux SDK. This will also take some time. Once the sdk
has been built you need to install it.

`sudo plinux/images/linux/sdk.sh`

You can choose where to install it. Now source the SDK
`. <install directory>/environment-setup-cortexa72-cortexa53-xilinx-linux`

This will define the SYSROOT and other ENV parameters

## Create the platform
`build platform`

This will create the Vitis Platform. This is still a bit of a WIP. It does create
a linux platform and attempts to create the new device tree, but that is
currently failing.

## Create the accelerators
`make accel`

This will built the SD card image. You don't have to override your SD image
every time. You really only need the BOOT.BIN, xclbin files, and any userspace
programs that use those accelerators.

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
setenv bootargs "earlycon console=ttyPS0,115200 clk_ignore_unused root=/dev/mmcblk0p2 rw rootwait"

#fatload mmc 0 0x01000000 Image.gz
sf probe
sf read 0x01000000 0x1420000 0x989680
unzip 0x01000000 0x00200000
booti 0x00200000 - 0x00100000
```

I then compress Image using gzip and copy it to the boot partition on the SD
card.
```
gzip -k Image
```

We now put the **BOOT.BIN** and the **Image.gz** in the QSPI flash. Use
the commands below in U-boot
```
fatload mmc 0 0x80000 BOOT.BIN
sf probe
sf update 0x80000 0 ${filesize}
fatload mmc 0 0x80000 Image.gz
sf update 0x80000 0x1420000 ${filesize}
fatload mmc 0 0x80000 boot.scr
sf update 0x80000 0x2000000 ${filesize}
```

Shut the unit off, configure it to boot from QSPI. Then boot. Loading the
BIT file, u-boot and the kernel should only take a few seconds now.

When u-boot starts up change the bootargs to:
```
setenv bootargs "earlycon console=ttyPS0,115200 clk_ignore_unused root=/dev/mmcblk0p2 rw rootwait quiet systemd.show_status=0 init=/sbin/bootchartd"
saveenv
```

You can also create a script to do this update:
```
setenv update_boot "fatload mmc 0 0x80000 BOOT.BIN;sf probe; sf update 0x80000 0 ${filesize};fatload mmc 0 0x80000 boot.scr;sf update 0x80000 0x2000000 ${filesize}"
```

The unit should boot within 15-20 seconds.

## U-boot changes
Remove as many drivers as possible.

Set this guy:
CONFIG_SYS_MALLOC_CLEAR_ON_INIT=n

## Profile

I can't get boot chart to work. It will generate a bootchart.tgz, but pybootchartgui.py doesn't work.
`setenv bootargs "earlycon console=ttyPS0,115200 clk_ignore_unused root=/dev/mmcblk0p2 rw rootwait cma=1536M quiet systemd.show_status=0 init=/sbin/bootchartd"`


To profile kernel calls do the following:
Change the kernel config **CONFIG_LOG_BUF_SHIFT** and increase it by one.
Boot to u-boot and type the following then boot
`setenv bootargs "earlycon console=ttyPS0,115200 clk_ignore_unused root=/dev/mmcblk0p2 rw rootwait cma=1536M quiet systemd.show_status=0 initcall_debug"`

One the unit is booted type:
`dmesg > boot.log`

Copy that file to the PC and in the kernel source tree run:
`scripts/bootgraph.pl boot.log > boot.svg`

# Start GStreamer app on startup
Petalinux uses SysV, which is terrible.

To start Gstreamer on the host use the following:
`gst-launch-1.0 udpsrc port=5000 ! 'application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)JPEG, a-framerate=(string)30.000000, payload=(int)26, ssrc=(uint)3167073459, timestamp-offset=(uint)2147749579, seqnum-offset=(uint)3676' ! rtpjitterbuffer latency=50 ! rtpjpegdepay ! jpegdec ! glimagesink`

# ZCU104 Boot switches
The image below shows the configuration for the boot switches
[switches](ZCU104_switches.png)